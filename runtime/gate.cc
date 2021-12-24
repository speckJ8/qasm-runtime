#include "gate.hpp"

#include <complex>
#include <cstring>
#include <immintrin.h>

namespace runtime {

using namespace std::complex_literals;

const math::unitary2d_t Gate::_I = { 
    1,  0,
    0,  1
};
const math::unitary2d_t Gate::_X = {
     0,  1,
    -1,  0
};
const math::unitary2d_t Gate::_Y = {
     0,  -1i,
    1i,  0
};
const math::unitary2d_t Gate::_Z = {
    1,  0,
    0, -1
};
const math::unitary4d_t Gate::_CX = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 0, 1,
    0, 0, 1, 0,
};

const Gate Gate::I  = Gate(Gate::_I, 1);
const Gate Gate::X  = Gate(Gate::_X, 1);
const Gate Gate::Y  = Gate(Gate::_Y, 1);
const Gate Gate::Z  = Gate(Gate::_Z, 1);
const Gate Gate::CX = Gate(Gate::_CX, 2);


Gate::Gate(double theta, double phi, double lambda) {
    math::cx_t i = (0. + 1i);
    auto ct2 = std::cos(theta/2);
    auto st2 = std::sin(theta/2);
    auto eippl = std::exp(((lambda + phi)/2)*i);
    auto eipml = std::exp(((lambda - phi)/2)*i);

    this->_unitary[0] = std::conj(ct2 * eippl);
    this->_unitary[1] = -1 * st2 * eipml;
    this->_unitary[2] = std::conj(st2 * eipml);
    this->_unitary[3] = ct2 * eipml;
}


};
