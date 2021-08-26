#ifndef __RUNTIME__STATE_H__
#define __RUNTIME__STATE_H__

#include <vector>
#include <unordered_map>

#include "qgate.h"

namespace runtime {

struct Register {
    // name used to define the registers
    std::string name;
    // how many qubits it holds
    unsigned int size;
};

/**
 * System quantum state during runtime.
 * The state consists of a set of registers.
 * */
class State {
public:

    State (std::vector<Register> registers);

    /**
     * Apply a quantum gate to a register.
     * The application of the gate mutates the state in the location
     * where `register_` is stored.
     * */
    void apply (Qgate gate, Register register_);

private:
    /**
     * The state vector holding the registers.
     * The registers are layed out in sequence in the vector
     * and we use the offset and size information in `_registers`
     * to be able to locate them.
     * */
    arma::cx_vec _state;

    /**
     * Metadata to allow us to locate a regiter in the state vector by name.
     * */
    struct _RegisterMeta {
        unsigned int offset;
        unsigned int size;
    };
    std::unordered_map<std::string, _RegisterMeta> _registers;

};

};


#endif // __RUNTIME__QUBIT_H__
