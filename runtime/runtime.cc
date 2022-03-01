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

#include "error.hpp"
#include "gate.hpp"
#include "lang/symbol_table.hpp"

namespace runtime {

static void declare_register(const std::shared_ptr<lang::VariableDeclaration>&);
static void declare_gate(const std::shared_ptr<lang::GateDeclaration>&);
static void execute_unitary(const std::shared_ptr<lang::UnitaryOperation>&);
static void execute_measure(const std::shared_ptr<lang::MeasureOperation>&);
static void execute_reset(const std::shared_ptr<lang::ResetOperation>&);
static void execute_barrier(const std::shared_ptr<lang::BarrierOperation>&);
static void execute_if_statement(const std::shared_ptr<lang::IfStatement>&);

static State _state;
static std::unordered_map<std::string, std::shared_ptr<lang::GateDeclaration>> _gates;

void execute(const lang::Program& program) {
    for (auto& stmt : program.statements) {
        if (auto declaration = std::dynamic_pointer_cast<lang::VariableDeclaration>(stmt)) {
            declare_register(declaration);
        } else if (auto declaration = std::dynamic_pointer_cast<lang::GateDeclaration>(stmt)) {
            declare_gate(declaration);
        } else if (auto unitary = std::dynamic_pointer_cast<lang::UnitaryOperation>(stmt)) {
            execute_unitary(unitary);
        } else if (auto reset = std::dynamic_pointer_cast<lang::ResetOperation>(stmt)) {
            execute_reset(reset);
        } else if (auto measure = std::dynamic_pointer_cast<lang::MeasureOperation>(stmt)) {
            execute_measure(measure);
        } else if (auto barrier = std::dynamic_pointer_cast<lang::BarrierOperation>(stmt)) {
            execute_barrier(barrier);
        } else if (auto ifstmt = std::dynamic_pointer_cast<lang::IfStatement>(stmt)) {
            execute_if_statement(ifstmt);
        } else if (auto comment = std::dynamic_pointer_cast<lang::Comment>(stmt)) {
            // nothing to do
        } else {
            throw Error("undefined statement (" + std::to_string(stmt->context.start_line) + ")");
        }
    }
}

const State& get_state() {
    return _state;
}

static void declare_register(const std::shared_ptr<lang::VariableDeclaration>& declaration) {
    if (declaration->type == lang::VariableDeclaration::Qbit) {
        _state.add_quantum_register(declaration->identifier, declaration->dimension);
    } else {
        _state.add_classical_register(declaration->identifier, declaration->dimension);
    }
}

static void declare_gate(const std::shared_ptr<lang::GateDeclaration>& declaration) {
    _gates[declaration->identifier] = declaration;
}

static void execute_unitary(const std::shared_ptr<lang::UnitaryOperation>&) {
}

static void execute_measure(const std::shared_ptr<lang::MeasureOperation>& measure) {
    auto qreg_name = measure->source.identifier;
    auto creg_name = measure->target.identifier;
    _state.measure(qreg_name, creg_name);
}

static void execute_reset(const std::shared_ptr<lang::ResetOperation>& reset) {
    auto qreg_name = reset->target.identifier;
    if (reset->target.index.has_value()) {
        auto index = reset->target.index.value();
        _state.reset_quantum_register_partial(qreg_name, index);
    } else {
        _state.reset_quantum_register(qreg_name);
    }
}

static void execute_barrier(const std::shared_ptr<lang::BarrierOperation>&) {
    // TODO: implement
}

static void execute_if_statement(const std::shared_ptr<lang::IfStatement>&) {
}
}  // namespace runtime
