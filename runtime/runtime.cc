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

#include "runtime.hpp"

#include <iostream>
#include <memory>

#include "gate.hpp"

namespace runtime {

void Runtime::execute(const lang::Program& program) {
    for (auto& stmt : program.statements) {
        if (auto declaration = std::dynamic_pointer_cast<lang::VariableDeclaration>(stmt)) {
            declare_register(declaration);
        } else if (auto declaration = std::dynamic_pointer_cast<lang::GateDeclaration>(stmt)) {
            _gates[declaration->identifier] = declaration;
        } else if (auto unitary = std::dynamic_pointer_cast<lang::UnitaryOperation>(stmt)) {
            execute_unitary(unitary);
        }
    }
}

const State& Runtime::get_state() const {
    return _state;
}

void Runtime::declare_register(const std::shared_ptr<lang::VariableDeclaration>& declaration) {
    if (declaration->type == lang::VariableDeclaration::Qbit) {
        _state.add_quantum_register(declaration->identifier, declaration->dimension);
    } else {
        _state.add_classical_register(declaration->identifier, declaration->dimension);
    }
}


void Runtime::execute_unitary(const std::shared_ptr<lang::UnitaryOperation>&) {}

void Runtime::execute_measure(const std::shared_ptr<lang::MeasureOperation>&) {}

void Runtime::execute_reset(const std::shared_ptr<lang::ResetOperation>&) {}

void Runtime::execute_if_statement(const std::shared_ptr<lang::IfStatement>&) {}
}
