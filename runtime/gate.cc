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
