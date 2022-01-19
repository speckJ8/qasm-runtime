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

#ifndef __RUNTIME__GATE_H__
#define __RUNTIME__GATE_H__

#include "math/unitary.hpp"
#include <complex>
#include <optional>
#include <vector>

namespace runtime {

class Gate {
public:
    /**
     * A gate can be defined in terms of other previously defined
     * gates, which we call SubGates here.
     * For example we can have the definition
     * gate foo a, b, c {
     *    bar a, b;
     *    baz c;
     * }
     * where bar and baz are subgates of foo.
     * The variable `SubGate.target_qubits` specifies the indices of
     * the qubits that the subgate acts on. For example, in the above
     * case the target indices of bar are 0 and 1.
     * */
    struct SubGate {
        Gate& gate;
        std::vector<unsigned int> target_qubits;
    };

    /**
     * Pauli matrix gates
     * */
    static const Gate I, X, Y, Z;
    /**
     * Controlled not gate
     * */
    static const Gate CX;

    /**
     * Create a single qubit gate defined by the unitary matrix
     *  _                                                                         _
     * | exp(-i(phi + lambda)/2)cos(theta/2)  -exp(-i(phi - lambda)/2)sin(theta/2) |
     * | exp(i(phi + lambda)/2)sin(theta/2)    exp(i(phi + lambda)/2)cos(theta/2)  |
     *  -                                                                         -
     * */
    Gate(float theta, float phi, float lambda);

    /**
     * Define a gate in terms of other gates.
     * The matrix of the new gate is obtained by appropriately
     * tensoring the matrices of the subgates.
     * For example
     * */
    Gate(unsigned int qubits, SubGate...);

    // State operator()(State& state);

private:
    /**
     * Pauli matrices
     * */
    static const math::unitary_t _I, _X, _Y, _Z;
    /**
     * Controlled not matrix
     * */
    static const math::unitary_t _CX;

    /**
     * The unitary matrix underlying the gate
     * */
    std::optional<math::unitary_t> _unitary;

    std::vector<SubGate> sub_gates;
};

}

#endif // __RUNTIME__GATE_H__
