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


#ifndef __RUNTIME__STATE_H__
#define __RUNTIME__STATE_H__

#include <iostream>
#include <map>
#include <vector>
#include "math/unitary.hpp"

namespace runtime {

class State {
private:
    bool _empty { true };
    // holds the tensor product of the 2d vectors for each quantum register
    math::vector_t _quantum_state { 1 };
    /**
     * Track the postion and offset of all the named quantum registers.
     * For example, for register definitions
     *     qreg a[2];
     *     qreg b[4];
     *     qreg c[1];
     * we obtain a register map
     *     {
     *         a: (0, 2),
     *         c: (2, 4),
     *         a: (6, 1),
     *     }
     * */
    std::map<std::string, std::tuple<size_t, size_t>> _quantum_registers;
    // keep the values of the classical registers
    std::map<std::string, std::vector<float>> _classical_registers;

public:
    State();

    void add_quantum_register(std::string name, size_t size);
    void add_classical_register(std::string name, size_t size);
};

};


#endif // __RUNTIME__STATE_H__
