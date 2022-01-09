/**
 * Copyright (c) 2022 Alcides Andrade <andrade.alcides.junior@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "statement.hpp"
#include "symbol_table.hpp"
#include "error.hpp"

namespace lang {

void IfStatement::declare_symbols() {
    conditional_operation->declare_symbols();
}

void OpaqueDeclaration::declare_symbols() {
    using namespace lang;

    auto symbol = symbol_table::get(identifier, true);
    if (symbol) {
        std::string msg = identifier + " has already been declared in line " +
                    std::to_string(symbol.value()->decl_line);
        throw Error(context, msg);
    }

    size_t nr_params = 0;
    if (parameters.has_value()) {
        nr_params = parameters.value().id_list.size();
    }
    size_t nr_args = arguments.id_list.size();
    auto gate = std::make_shared<symbol_table::Gate>(identifier,
                                                     nr_params,
                                                     nr_args,
                                                     context.start_line);
    symbol_table::declare(gate);
    symbol_table::push_new_scope();
    if (parameters.has_value()) {
        auto params = parameters.value().id_list;
        auto position = 0;
        for (auto param : params) {
            auto symbol = symbol_table::get(param, true);
            if (symbol) {
                std::string msg = param + " has already been declared in line " +
                            std::to_string(symbol.value()->decl_line);
                throw Error(context, msg);
            }
            auto p = std::make_shared<symbol_table::GateParameter>(param,
                                                                   position++,
                                                                   context.start_line);
            symbol_table::declare(p);
        }
    }

    auto position = 0;
    auto args = arguments.id_list;
    for (auto arg : args) {
        auto symbol = symbol_table::get(arg, true);
        if (symbol) {
            std::string msg = arg + " has already been declared in line " +
                        std::to_string(symbol.value()->decl_line);
            throw Error(context, msg);
        }
        auto a = std::make_shared<symbol_table::GateArgument>(arg,
                                                              position++,
                                                              context.start_line);
        symbol_table::declare(a);
    }

    symbol_table::pop_and_save_scope(identifier);
}

void VariableDeclaration::declare_symbols() {
    using namespace lang;

    auto symbol = symbol_table::get(identifier, true);
    if (symbol) {
        std::string msg = identifier + " has already been declared in line " +
                    std::to_string(symbol.value()->decl_line);
        throw Error(context, msg);
    }

    auto vector_type = type == DeclarationType::Qbit ?
        symbol_table::Vector::Qbit : symbol_table::Vector::Cbit;
    auto vector = std::make_shared<symbol_table::Vector>(vector_type, identifier, 
                                                         dimension,
                                                         context.start_line);
    symbol_table::declare(vector);
}

void GateDeclaration::declare_symbols() {
    using namespace lang;

    auto symbol = symbol_table::get(identifier, true);
    if (symbol) {
        std::string msg = identifier + " has already been declared in line " +
                    std::to_string(symbol.value()->decl_line);
        throw Error(context, msg);
    }

    size_t nr_params = 0;
    if (parameters.has_value()) {
        nr_params = parameters.value().id_list.size();
    }
    size_t nr_args = arguments.id_list.size();
    auto gate = std::make_shared<symbol_table::Gate>(identifier,
                                                     nr_params,
                                                     nr_args,
                                                     context.start_line);
    symbol_table::declare(gate);
    symbol_table::push_new_scope();
    if (parameters.has_value()) {
        auto params = parameters.value().id_list;
        auto position = 0;
        for (auto param : params) {
            auto symbol = symbol_table::get(param, true);
            if (symbol) {
                std::string msg = param + " has already been declared in line " +
                            std::to_string(symbol.value()->decl_line) ;
                throw Error(context, msg);
            }
            auto p = std::make_shared<symbol_table::GateParameter>(param, position++,
                                                                   context.start_line);
            symbol_table::declare(p);
        }
    }

    auto position = 0;
    auto args = arguments.id_list;
    for (auto arg : args) {
        auto symbol = symbol_table::get(arg, true);
        if (symbol) {
            std::string msg = arg + " has already been declared in line " +
                    std::to_string(symbol.value()->decl_line);
            throw Error(context, msg);
        }
        auto p = std::make_shared<symbol_table::GateArgument>(arg,
                                                              position++,
                                                              context.start_line);
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
void IfStatement::verify() {
    variable.verify();
    target_to_compare->verify();
    conditional_operation->verify();
}

void GateDeclaration::verify() {
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
void MeasureOperation::verify() {
    source.verify();
    target.verify();
    if (!source.is_qubit()) {
        std::string msg = "the source '" + source.identifier + 
                    "' of the measure opeation must be a qubit";
        throw Error(context, msg);
    } else if (target.is_qubit()) {
        std::string msg = "the target '" + target.identifier + 
                                  "' of the measure opeation must be a cbit";
        throw Error(context, msg);
    }
}

/**
 * Verify that the reset target is a qubit or a cbit.
 * */
void ResetOperation::verify() {
    target.verify();
}

/**
 * Verify that the operator is called with the appropriate parameters
 * and arguments
 * */
void UnitaryOperation::verify() {
    if (op == Operator::Defined) {
        auto symbol = symbol_table::get(operator_name);
        if (!symbol) {
            throw Error(context, "Operator " + operator_name + " is not defined");
        }
        auto gate = std::dynamic_pointer_cast<symbol_table::Gate>(symbol.value());
        if (!gate) {
            std::string msg = "Cannot use symbol " + operator_name  + ", declared in line " +
                          std::to_string(symbol.value()->decl_line) + ", as an operator";
            throw Error(context, msg);
        }

        // verify parameters
        if (!expression_list) {
            if (gate->nr_parameters != 0) {
                std::string msg = "Operator " + operator_name + " expected " +
                              std::to_string(gate->nr_parameters) +
                              " parameters, but no parameters were passed";
                throw Error(context, msg);
            }
        } else if (gate->nr_parameters != expression_list.value().expression_list.size()) {
            auto n = expression_list.value().expression_list.size();
            std::string msg = "Operator " + operator_name + " expected " +
                          std::to_string(gate->nr_parameters) +
                          " parameters, but " + std::to_string(n) +
                          " parameters were passed";
            throw Error(context, msg);
        }

        // verify arguments
        if (gate->nr_arguments != argument_list.mixed_list.size()) {
            auto n = argument_list.mixed_list.size();
            std::string msg = "Operator " + operator_name + " expected " +
                          std::to_string(gate->nr_arguments) +
                          " arguments, but " + std::to_string(n) +
                          " arguments were passed";
            throw Error(context, msg);
        }
    }
    else if (op == Operator::U) {
        if (!expression_list || expression_list.value().expression_list.size() != 3) {
            auto n = expression_list.value().expression_list.size();
            std::string msg = "Operator U expects 3 parameters, but " +
                        std::to_string(n) + " parameters were passed";
            throw Error(context, msg);
        }
    } else {
        if (expression_list && expression_list.value().expression_list.size() != 0) {
            auto n = expression_list.value().expression_list.size();
            std::string msg = "Operator CX expects no parameters, but " +
                        std::to_string(n) + " parameters were passed";
            throw Error(context, msg);
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
            std::string msg = variable.identifier + " is not a qubit. " +
                        "Operator arguments must be qubits";
            throw Error(context, msg);
        }
    }
}

/**
 * Verify that the names in the operation are declared as variables
 * */
void BarrierOperation::verify() {
    for (auto& variable : variables.mixed_list) {
        variable.verify();
    }
}

}
