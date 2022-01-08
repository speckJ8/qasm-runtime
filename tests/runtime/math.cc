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
#include <iostream>
#include <tuple>
#include <vector>
#include "runtime/math/math.hpp"

using namespace runtime::math;
using namespace std::complex_literals;

TEST(Math, VecApply) {
    std::vector<std::tuple<unitary_t, vector_t, vector_t>> test_data = {
        {
            {2, {
                1.f + 2if, 3.f + 4if,
                5.f + 6if, 7.f + 8if,
            }},
            { 2.f + 1if, 1.f + 2if },
            { -5.f + 15if, -5.f + 39if },
        },
        {
            {4, {
                1.f, 0, 0, 0,
                0, 1.f, 0, 0,
                0, 0, 1.f, 0,
                0, 0, 0, 1.f,
            }},
            { 2.f + 1if, 1.3f + 2if, -5.2f, 0.4if },
            { 2.f + 1if, 1.3f + 2if, -5.2f, 0.4if },
        },
    };
    for (auto& [ mat, vec, res ] : test_data) {
        vector_t _res = mat*vec;
        EXPECT_EQ(res == _res, true)
            << "expectedResult=" << res << std::endl
            << "obtainedResult=" << _res << std::endl;
    }
}
