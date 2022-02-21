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

#include "expression.hpp"
#include "error.hpp"
#include "symbol_table.hpp"

#include <iostream>

namespace lang {

void MinusExpression::verify() {
    negated_expression->verify();
}

/**
 * Check validity of variable call:
 * * check if variable is defined
 * * check if indexing is valid
 * */
void Variable::verify() {
    auto symbol = symbol_table::get(identifier);
    if (!symbol) {
        throw Error(context, "Variable " + identifier + " is not defined");
    }

    auto variable = std::dynamic_pointer_cast<symbol_table::Vector>(symbol.value());
    if (variable) {
        if (index && index.value() >= variable->dimension) {
            std::string msg =  "Variable " + identifier + " is vector of dimension " +
                    std::to_string(variable->dimension) + " and cannot be indexed at " + 
                    std::to_string(index.value());
            throw Error(context, msg);
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

    std::string msg = "Cannot use symbol " + identifier + ", declared in line " +
              std::to_string(symbol.value()->decl_line) + ", as a variable";
    throw Error(context, msg);
}

/**
 * Check if the variable was declared as a qreg
 * */
bool Variable::is_qubit() {
    auto symbol = symbol_table::get(identifier);
    if (!symbol) {
        throw Error(context, "Variable " + identifier + " is not defined");
    }

    // check if it a qubit register
    auto variable = std::dynamic_pointer_cast<symbol_table::Vector>(symbol.value());
    if (variable) {
        return variable->type == symbol_table::Vector::Qbit;
    }

    // if it is a gate argument then it is a qubit
    auto gate_argument = std::dynamic_pointer_cast<symbol_table::GateArgument>(symbol.value());
    if (gate_argument) {
        return true;
    }

    // if it is a gate argument then it is not a qubit
    auto gate_parameter = std::dynamic_pointer_cast<symbol_table::GateParameter>(symbol.value());
    if (gate_parameter) {
        return false;
    }

    std::string msg = "Cannot use symbol " + identifier + ", declared in line " +
              std::to_string(symbol.value()->decl_line) + ", as a variable";
    throw Error(context, msg);
}

void verify_binary(std::shared_ptr<Expression>& left,
                   std::shared_ptr<Expression>& right,
                   std::string name) {
    using namespace lang;

    left->verify();
    right->verify();

    if (!left->is_atom()) {
        std::string msg = name + " operations cannot be performed on vectors. " +
                left->to_string() + " is a vector";
        throw Error(left->context, msg);
    } else if (!right->is_atom()) {
        std::string msg = name + " operations cannot be performed on vectors. " +
                right->to_string() + " is a vector";
        throw Error(right->context, msg);
    }

    if (left->is_qubit()) {
        std::string msg = name + " operations cannot be performed on qubits. " +
                left->to_string() + " is a qubit";
        throw Error(left->context, msg);
    } else if (right->is_qubit()) {
        std::string msg = name + " operations cannot be performed on vectors. " +
                    right->to_string() + " is a qubit";
        throw Error(right->context, msg);
    }
}

void AdditionExpression::verify() {
    verify_binary(left, right, "Addition");
}

void SubtractionExpression::verify() {
    verify_binary(left, right, "Subtraction");
}

void MultiplicationExpression::verify() {
    verify_binary(left, right, "Multiplication");
}

void DivisionExpression::verify() {
    verify_binary(left, right, "Division");
}

void ExponentiationExpression::verify() {
    verify_binary(left, right, "Exponentiation");
}

void UnaryOperation::verify() {
    target->verify();

    if (!target->is_atom()) {
        std::string msg = "Unary operations cannot be performed on vectors. " +
                    target->to_string() + " is a vector";
        throw Error(target->context, msg);
    }
    if (target->is_qubit()) {
        std::string msg = "Unary operations cannot be performed on qubits. " +
                    target->to_string() + " is a qubit";
        throw Error(target->context, msg);
    }
}

}
