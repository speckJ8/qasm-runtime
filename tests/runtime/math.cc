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
