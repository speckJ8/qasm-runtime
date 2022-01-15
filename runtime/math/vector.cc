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
