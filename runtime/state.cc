#include "state.hpp"
#include <cassert>
#include <cmath>

namespace runtime {

void State::add_register(std::string name, size_t size) {
    assert(size > 0);
    math::vector_t new_state_registers(std::exp2l(size));
    new_state_registers[0] = 1.f;
    if (__builtin_expect(_empty, 0)) {
        _state = std::move(new_state_registers);
    } else {
        _state = _state.tensor(new_state_registers);
    }
    _registers[name] = { _registers.size(), size };
}

};
