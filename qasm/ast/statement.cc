#include "statement.h"
#include "qasm/symbol_table.h"

void qasm::ast::IfStatement::declare_symbols () {
    conditional_operation->declare_symbols();
}

void qasm::ast::OpaqueDeclaration::declare_symbols () {
    using namespace qasm;

    auto symbol = symbol_table::get(identifier, true);
    if (symbol) {
        throw sema::SemanticError(identifier + " has already been declared in line " +
                                  std::to_string(symbol.value()->decl_line),
                                  context.line, context.column);
    }

    unsigned int nr_params = 0;
    if (parameters.has_value()) {
        nr_params = parameters.value().id_list.size();
    }
    unsigned int nr_args = arguments.id_list.size();
    auto gate =
        std::make_shared<symbol_table::Gate>(identifier, nr_params, nr_args, context.line);
    symbol_table::declare(gate);

    symbol_table::push_new_scope();

    if (parameters.has_value()) {
        auto params = parameters.value().id_list;
        auto position = 0;
        for (auto param : params) {
            auto symbol = symbol_table::get(param, true);
            if (symbol) {
                throw sema::SemanticError(param + " has already been declared in line " +
                                          std::to_string(symbol.value()->decl_line),
                                          context.line, context.column);
            }
            auto p = std::make_shared<symbol_table::GateParameter>(param, position++,
                                                                   context.line);
            symbol_table::declare(p);
        }
    }

    auto position = 0;
    auto args = arguments.id_list;
    for (auto arg : args) {
        auto symbol = symbol_table::get(arg, true);
        if (symbol) {
            throw sema::SemanticError(arg + " has already been declared in line " +
                                      std::to_string(symbol.value()->decl_line),
                                      context.line, context.column);
        }
        auto a = 
            std::make_shared<symbol_table::GateArgument>(arg, position++, context.line);
        symbol_table::declare(a);
    }

    symbol_table::pop_and_save_scope(identifier);
}

void qasm::ast::VariableDeclaration::declare_symbols () {
    using namespace qasm;

    auto symbol = symbol_table::get(identifier, true);
    if (symbol) {
        throw sema::SemanticError(identifier + " has already been declared in line " +
                                  std::to_string(symbol.value()->decl_line),
                                  context.line, context.column);
    }

    auto vector_type = type == DeclarationType::Qbit ?
        symbol_table::Vector::Qbit : symbol_table::Vector::Cbit;
    auto vector = std::make_shared<symbol_table::Vector>(vector_type, identifier, 
                                                         dimension,
                                                         context.line);
    symbol_table::declare(vector);
}

void qasm::ast::GateDeclaration::declare_symbols () {
    using namespace qasm;

    auto symbol = symbol_table::get(identifier, true);
    if (symbol) {
        throw sema::SemanticError(identifier + " has already been declared in line " +
                                  std::to_string(symbol.value()->decl_line),
                                  context.line, context.column);
    }

    unsigned int nr_params = 0;
    if (parameters.has_value()) {
        nr_params = parameters.value().id_list.size();
    }
    unsigned int nr_args = arguments.id_list.size();
    auto gate =
        std::make_shared<symbol_table::Gate>(identifier, nr_params, nr_args, context.line);
    symbol_table::declare(gate);

    symbol_table::push_new_scope();

    if (parameters.has_value()) {
        auto params = parameters.value().id_list;
        auto position = 0;
        for (auto param : params) {
            auto symbol = symbol_table::get(param, true);
            if (symbol) {
                throw sema::SemanticError(param + " has already been declared in line " +
                                          std::to_string(symbol.value()->decl_line),
                                          context.line, context.column);
            }
            auto p = std::make_shared<symbol_table::GateParameter>(param, position++,
                                                                   context.line);
            symbol_table::declare(p);
        }
    }

    auto position = 0;
    auto args = arguments.id_list;
    for (auto arg : args) {
        auto symbol = symbol_table::get(arg, true);
        if (symbol) {
            throw sema::SemanticError(arg + " has already been declared in line " +
                                      std::to_string(symbol.value()->decl_line),
                                      context.line, context.column);
        }
        auto p = std::make_shared<symbol_table::GateArgument>(arg, position++, context.line);
        symbol_table::declare(p);
    }

    for (auto&& operation : body) {
        operation->declare_symbols();
    }

    symbol_table::pop_and_save_scope(identifier);
}

/**
 * Verify the condition and body of the if statement
 * */
void qasm::ast::IfStatement::verify () {
    variable.verify();
    target_to_compare->verify();
    conditional_operation->verify();
}

void qasm::ast::GateDeclaration::verify () {
    symbol_table::restore_scope(identifier);
    for (auto&& stmt : body) {
        stmt->verify();
    }
    symbol_table::pop_and_save_scope(identifier);
}

/**
 * Verify that what is being measured is a qubit register and that
 * the target is a cbit register.
 * */
void qasm::ast::MeasureOperation::verify () {
    source.verify();
    target.verify();
    if (!source.is_qubit()) {
        throw sema::SemanticError("the source '" + source.identifier + 
                                  "' of the measure opeation must be a qubit",
                                  context.line, context.column);
    } else if (target.is_qubit()) {
        throw sema::SemanticError("the target '" + target.identifier + 
                                  "' of the measure opeation must be a cbit",
                                  context.line, context.column);
    }
}

/**
 * Verify that the reset target is a qubit or a cbit.
 * */
void qasm::ast::ResetOperation::verify () {
    target.verify();
}

/**
 * Verify that the operator is called with the appropriate parameters
 * and arguments
 * */
void qasm::ast::UnitaryOperation::verify () {
    if (op == Operator::Defined) {
        auto symbol = symbol_table::get(operator_name);
        if (!symbol) {
            throw sema::SemanticError("Operator " + operator_name + " is not defined",
                                      context.line, context.column);
        }
        auto gate = std::dynamic_pointer_cast<symbol_table::Gate>(symbol.value());
        if (!gate) {
            throw sema::SemanticError("Cannot use symbol " + operator_name  +
                                      ", declared in line " +
                                      std::to_string(symbol.value()->decl_line) +
                                      ", as an operator",
                                      context.line, context.column);
        }

        // verify parameters
        if (!expression_list) {
            if (gate->nr_parameters != 0) {
                throw sema::SemanticError("Operator " + operator_name + " expected " +
                                          std::to_string(gate->nr_parameters) +
                                          " parameters, but no parameters were passed",
                                          context.line, context.column);
            }
        } else if (gate->nr_parameters != expression_list.value().expression_list.size()) {
            auto n = expression_list.value().expression_list.size();
            throw sema::SemanticError("Operator " + operator_name + " expected " +
                                      std::to_string(gate->nr_parameters) +
                                      " parameters, but " + std::to_string(n) +
                                      " parameters were passed",
                                      context.line, context.column);
        }

        // verify arguments
        if (gate->nr_arguments != argument_list.mixed_list.size()) {
            auto n = argument_list.mixed_list.size();
            throw sema::SemanticError("Operator " + operator_name + " expected " +
                                      std::to_string(gate->nr_arguments) +
                                      " arguments, but " + std::to_string(n) +
                                      " arguments were passed",
                                      context.line, context.column);
        }
    }
    else if (op == Operator::U) {
        if (!expression_list || expression_list.value().expression_list.size() != 3) {
            auto n = expression_list.value().expression_list.size();
            throw sema::SemanticError("Operator U expects 3 parameters, but " +
                                      std::to_string(n) + " parameters were passed",
                                      context.line, context.column);
        }
    } else {
        if (expression_list && expression_list.value().expression_list.size() != 0) {
            auto n = expression_list.value().expression_list.size();
            throw sema::SemanticError("Operator CX expects no parameters, but " +
                                      std::to_string(n) + " parameters were passed",
                                      context.line, context.column);
        }
    }

    if (expression_list) {
        for (auto&& param : expression_list.value().expression_list) {
            param->verify();
        }
    }

    for (auto& variable : argument_list.mixed_list) {
        variable.verify();
        if (!variable.is_qubit()) {
            throw sema::SemanticError(variable.identifier + " is not a qubit. " +
                                      "Operator arguments must be qubits",
                                      context.line, context.column);
        }
    }
}

/**
 * Verify that the names in the operation are declared as variables
 * */
void qasm::ast::BarrierOperation::verify () {
    for (auto& variable : variables.mixed_list) {
        variable.verify();
    }
}
