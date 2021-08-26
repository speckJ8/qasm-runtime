#include "expression.h"
#include "qasm/sema_error.h"
#include "qasm/symbol_table.h"
#include <iostream>

void qasm::ast::MinusExpression::verify () {
    negated_expression->verify();
}

/**
 * Check validity of variable call:
 * * check if variable is defined
 * * check if indexing is valid
 * */
void qasm::ast::Variable::verify () {
    auto symbol = symbol_table::get(identifier);
    if (!symbol) {
        throw sema::SemanticError("Variable " + identifier + " is not defined",
                                  context.line, context.column);
    }

    auto variable = std::dynamic_pointer_cast<symbol_table::Vector>(symbol.value());
    if (variable) {
        if (index && index.value() >= variable->dimension) {
            throw sema::SemanticError("Variable " + identifier +
                    " is vector of dimension " +
                    std::to_string(variable->dimension) + " and cannot be indexed at " + 
                    std::to_string(index.value()),
                    context.line, context.column);
        }
        return;
    }

    auto gate_argument =
        std::dynamic_pointer_cast<symbol_table::GateArgument>(symbol.value());
    if (gate_argument) {
        return;
    }

    auto gate_parameter =
        std::dynamic_pointer_cast<symbol_table::GateParameter>(symbol.value());
    if (gate_parameter) {
        return;
    }

    throw sema::SemanticError("Cannot use symbol " + identifier + 
                              ", declared in line " +
                              std::to_string(symbol.value()->decl_line) +
                              ", as a variable",
                              context.line, context.column);
}

/**
 * Check if the variable was declared as a qreg
 * */
bool qasm::ast::Variable::is_qubit () {
    auto symbol = symbol_table::get(identifier);
    if (!symbol) {
        throw sema::SemanticError("Variable " + identifier + " is not defined",
                                  context.line, context.column);
    }

    // check if it a qubit register
    auto variable = std::dynamic_pointer_cast<symbol_table::Vector>(symbol.value());
    if (variable) {
        return variable->type == symbol_table::Vector::Qbit;
    }

    // if it is a gate argument then it is a qubit
    auto gate_argument =
        std::dynamic_pointer_cast<symbol_table::GateArgument>(symbol.value());
    if (gate_argument) {
        return true;
    }

    // if it is a gate argument then it is a qubit
    auto gate_parameter =
        std::dynamic_pointer_cast<symbol_table::GateParameter>(symbol.value());
    if (gate_parameter) {
        return false;
    }

    // it is not a variable
    throw sema::SemanticError("Cannot use symbol " + identifier + 
                              ", declared in line " +
                              std::to_string(symbol.value()->decl_line) +
                              ", as a variable",
                              context.line, context.column);
}

void verify_binary (std::shared_ptr<qasm::ast::Expression>& left,
                    std::shared_ptr<qasm::ast::Expression>& right,
                    std::string name) {
    using namespace qasm;

    left->verify();
    right->verify();

    if (!left->is_atom()) {
        throw sema::SemanticError(name + " operations cannot be performed on vectors. " +
                                      left->to_string() + " is a vector",
                                  left->context.line, left->context.column);
    } else if (!right->is_atom()) {
        throw sema::SemanticError(name + " operations cannot be performed on vectors. " +
                                      right->to_string() + " is a vector",
                                  right->context.line, right->context.column);
    }

    if (left->is_qubit()) {
        throw sema::SemanticError(name + " operations cannot be performed on qubits. " +
                                      left->to_string() + " is a qubit",
                                  left->context.line, left->context.column);
    } else if (right->is_qubit()) {
        throw sema::SemanticError(name + " operations cannot be performed on vectors. " +
                                      right->to_string() + " is a qubit",
                                  right->context.line, right->context.column);
    }
}

void qasm::ast::AdditionExpression::verify () {
    verify_binary(left, right, "Addition");
}

void qasm::ast::SubtractionExpression::verify () {
    verify_binary(left, right, "Subtraction");
}

void qasm::ast::MultiplicationExpression::verify () {
    verify_binary(left, right, "Multiplication");
}

void qasm::ast::DivisionExpression::verify () {
    verify_binary(left, right, "Division");
}

void qasm::ast::ExponentiationExpression::verify () {
    verify_binary(left, right, "Exponentiation");
}

void qasm::ast::UnaryOperation::verify () {
    target->verify();

    if (!target->is_atom()) {
        throw sema::SemanticError("Unary operations cannot be performed on vectors. " +
                                      target->to_string() + " is a vector",
                                  target->context.line, target->context.column);
    }
    if (target->is_qubit()) {
        throw sema::SemanticError("Unary operations cannot be performed on qubits. " +
                                      target->to_string() + " is a qubit",
                                  target->context.line, target->context.column);
    }
}
