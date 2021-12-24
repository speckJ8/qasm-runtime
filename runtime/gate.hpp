#ifndef __RUNTIME__GATE_H__
#define __RUNTIME__GATE_H__

#include <complex>
#include <vector>
#include "math/math.hpp"

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
        Gate *gate;
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
    Gate(double theta, double phi, double lambda);

    /**
     * Define a gate in terms of other gates.
     * The matrix of the new gate is obtained by appropriately
     * tensoring the matrices of the subgates.
     * For example
     * */
    Gate(unsigned int qubits, SubGate...);

    // State operator()(State& state);

private:
    Gate(math::unitary2d_t const unitary, unsigned int qubits): _qubits(qubits) {
        this->_unitary.assign(unitary, unitary + 4);
    };
    Gate(math::unitary_t const unitary, unsigned int qubits) :
        _unitary(unitary), _qubits(qubits) {};

    /**
     * Pauli matrices
     * */
    static const math::unitary2d_t _I, _X, _Y, _Z;
    /**
     * Controlled not matrix
     * */
    static const math::unitary4d_t _CX;

    /**
     * 2x2 unitary underlying the gate
     * */
    math::unitary_t _unitary;
    /**
     * Dimension of the gate in terms of qubits.
     * It must be satisfied that _unitary.size() == (2^(_qubits))^2
     * */
    unsigned int _qubits;
};

};


#endif // __RUNTIME__GATE_H__
