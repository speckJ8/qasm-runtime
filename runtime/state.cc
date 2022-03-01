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

#include "state.hpp"

#include <cassert>
#include <cmath>

#include "error.hpp"

namespace runtime {

void State::add_quantum_register(std::string name, size_t size) {
    assert(size > 0);
    size_t dim = std::exp2l(size);
    size_t offset = 0;
    math::vector_t new_state_registers(dim);
    new_state_registers[0] = 1.f;
    if (__builtin_expect(_empty, 0)) {
        _quantum_state = std::move(new_state_registers);
        _empty = false;
    } else {
        _quantum_state = _quantum_state.tensor(new_state_registers);
        auto last_off_and_size = _quantum_registers.end()->second;
        offset = std::get<0>(last_off_and_size) + std::get<1>(last_off_and_size);
    }
    _quantum_registers[name] = { offset, size };
}

void State::add_classical_register(std::string name, size_t size) {
    _classical_registers[name] = std::vector<bool>(size, false);
}

void State::set_classical_register(std::string name, std::vector<bool> value) {
    _classical_registers[name] = value;
}

void State::reset_quantum_register(std::string name) {
    auto qreg = _quantum_registers.find(name);
    if (qreg == _quantum_registers.end()) {
        throw Error("undefined quantum register `" + name + "`");
    }
    auto [offset, size] = qreg->second;
    _quantum_state.reset(offset, size);
}

void State::reset_quantum_register_partial(std::string name, size_t index) {
    auto qreg = _quantum_registers.find(name);
    if (qreg == _quantum_registers.end()) {
        throw Error("undefined quantum register `" + name + "`");
    }
    auto [offset, _] = qreg->second;
    _quantum_state.reset(offset+index, 1);
}

void State::measure(std::string qreg_name, std::string creg_name) {
    auto qreg = _quantum_registers.find(qreg_name);
    auto creg = _classical_registers.find(creg_name);
    if (qreg == _quantum_registers.end()) {
        throw Error("undefined quantum register `" + qreg_name + "`");
    }
    if (creg == _classical_registers.end()) {
        throw Error("undefined classical register `" + creg_name + "`");
    }
    auto [offset, size] = qreg->second;
    _quantum_state.measure(offset, size, creg->second);
}
};
