#include "gate.hpp"

#include <complex>
#include <cstring>
#include <immintrin.h>

namespace runtime {

using namespace std::complex_literals;

const math::unitary_t Gate::_I = { 1,  0, 0,  1 };
const math::unitary_t Gate::_X = { 0,  1, -1,  0 };
const math::unitary_t Gate::_Y = { 0,  -1if, 1if,  0 };
const math::unitary_t Gate::_Z = { 1,  0, 0, -1 };
const math::unitary_t Gate::_CX = {
    1.f, 0, 0, 0,
    0, 1.f, 0, 0,
    0, 0, 0, 1.f,
    0, 0, 1.f, 0,
};

Gate::Gate(float theta, float phi, float lambda) {
    math::cx_t i = (0.f + 1if);
    auto ct2 = std::cos(theta/2);
    auto st2 = std::sin(theta/2);
    auto eippl = std::exp(((lambda + phi)/2)*i);
    auto eipml = std::exp(((lambda - phi)/2)*i);
    this->_unitary = {
        std::conj(ct2 * eippl), -1 * st2 * eipml,
        std::conj(st2 * eipml), ct2 * eipml,
    };
}

};
