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
    // global state vector
    math::vector_t _state { 1 };
    /**
     * Track the postion and offset of all the named registers.
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
    std::map<std::string, std::tuple<size_t, size_t>> _registers;

public:
    State();

    void add_register(std::string name, size_t size);
};

};


#endif // __RUNTIME__STATE_H__
