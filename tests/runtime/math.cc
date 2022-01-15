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

#include <gtest/gtest.h>
#include <initializer_list>
#include <iostream>
#include <tuple>
#include <vector>
#include "runtime/math/unitary.hpp"
#include "runtime/math/vector.hpp"

using namespace runtime::math;
using namespace std::complex_literals;

typedef std::initializer_list<runtime::math::cx_t> cxv_t;

TEST(Math, VecApply) {
    std::vector<std::tuple<cxv_t, cxv_t, cxv_t>> test_data = {
        {
            {
                1.f + 2if, 3.f + 4if,
                5.f + 6if, 7.f + 8if,
            },
            { 2.f + 1if, 1.f + 2if },
            { -5.f + 15if, -5.f + 39if },
        },
        {
            {
                1.f, 0, 0, 0,
                0, 1.f, 0, 0,
                0, 0, 1.f, 0,
                0, 0, 0, 1.f,
            },
            { 2.f + 1if, 1.3f + 2if, -5.2f, 0.4if },
            { 2.f + 1if, 1.3f + 2if, -5.2f, 0.4if },
        },
    };
    for (auto& [ mat, vec, res ] : test_data) {
        unitary_t _mat(mat);
        vector_t _vec(vec);
        vector_t _res = _mat*_vec;
        vector_t _eres(res);
        EXPECT_EQ(_eres == _res, true)
            << "expectedResult=" << _eres << std::endl
            << "obtainedResult=" << _res << std::endl;
    }
}

TEST(Math, MatMul) {
    std::vector<std::tuple<cxv_t, cxv_t, cxv_t>> test_data = {
        {
            {
                1.f + 2if,      3.f + 4if,  .5if,        12.4f - 4if,
                5.f + 6if,      7.f + 8if,  0,           -0.3f,
                0,              0,          -3.f + 1if,  1if,
                -3.5f + 1.2if,  0,          100if,       0
            },
            {
                 2, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1,
            },
            {
                2.f + 4if,     3.f + 4if,  .5if,        12.4f - 4if,
                10.f + 12if,   7.f + 8if,  0,           -0.3f,
                0,             0,          -3.f + 1if,  1if,
                -7.f + 2.4if,  0,          100if,       0
            },
        },
    };
    for (auto& [ mat_a, mat_b, res ] : test_data) {
        unitary_t _res = unitary_t(mat_a)*unitary_t(mat_b);
        EXPECT_EQ(_res, unitary_t(res));
    }
}

TEST(Math, VecTensor) {
    std::vector<std::tuple<cxv_t, cxv_t, cxv_t>> test_data = {
        {
            { 1.f, 2.f },
            { 3.f, 4.f, 5.f },
            { 3.f, 4.f, 5.f, 6.f, 8.f, 10.f }
        },
    };
    for (auto& [ vec_a, vec_b, res ] : test_data) {
        vector_t _res = vector_t(vec_a).tensor(vector_t(vec_b));
        EXPECT_EQ(_res, vector_t(res));
    }
}
