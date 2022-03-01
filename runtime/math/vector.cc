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

#include "vector.hpp"
#include <cassert>
#include <random>

/**
 * Compute the tensor product of two vectors
 * */
void vec_tensor(const runtime::math::Vector& vec_a,
                const runtime::math::Vector& vec_b,
                runtime::math::Vector& res);
// defined in vec_tensor.S
extern "C" void vec_tensor__avx(const void* vec_a, size_t size_vec_a,
                                const void* vec_b, size_t size_vec_b,
                                void* res);

namespace runtime {
namespace math {

Vector Vector::tensor(const Vector& other) const {
    Vector res(this->size()*other.size());
#ifdef USE_SIMD
    if (__builtin_cpu_supports("avx") && this->size() >= 4) {
        vec_tensor__avx(other.ptr(), other.size(), this->ptr(), this->size(), res.ptr());
    } else {
        vec_tensor(*this, other, res);
    }
#else
    vec_tensor(*this, other, res);
#endif
    return res;
}

void Vector::reset(size_t offset, size_t size) {
    size_t step = std::exp2l(offset);
    size_t r = step*std::exp2l(size);
    for (size_t i = 0; i < _size; i += step) {
        if (i % r != 0) {
            for (size_t j = i; j < i+step; j++) {
                _entries[j] = 0;
            }
        }
    }
    normalize();
}

void Vector::measure(size_t offset, size_t size, std::vector<bool>& res) {
    size_t step = std::exp2l(offset);
    size_t block = std::exp2l(size);
    size_t r = step*block;
    std::vector<double> prob_measure(block);
    for (size_t i = 0; i < _size; i += step) {
        for (size_t j = i; j < i+step; j++) {
            auto c = std::abs(_entries[j]);
            prob_measure[(i/step)%block] += c*c;
        }
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<size_t> distr(prob_measure.begin(), prob_measure.end());
    size_t m = distr(gen);
    for (size_t i = 0; i < _size; i += step) {
        for (size_t j = i; j < i+step; j++) {
            if (i % r != m) {
                _entries[j] = 0;
            }
        }
    }
    normalize();
    for (size_t i = 0; i < size; i++) {
        res[i] = (m & 1) == 1;
        m >>= 1;
    }
}

void Vector::measure(std::vector<bool>& res) {
    measure(0, std::log2l(size()), res);
}

void Vector::normalize() {
    float norm = 0;
    for (size_t i = 0; i < _size; i++) {
        auto abs = std::abs(_entries[i]);
        norm += abs * abs;
    }
    norm = std::sqrt(norm);
    for (size_t i = 0; i < _size; i++) {
        _entries[i] = _entries[i] / norm;
    }
}

}
}

void vec_tensor(const runtime::math::Vector& vec_a,
                const runtime::math::Vector& vec_b,
                runtime::math::Vector& res)
{
    for (size_t i = 0; i < vec_a.size(); i++) {
        for (size_t j = 0; j < vec_b.size(); j++) {
            res[i*vec_b.size() + j] = vec_a[i]*vec_b[j];
        }
    }
}
