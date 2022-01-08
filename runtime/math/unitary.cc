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

#include "unitary.hpp"

/**
 * Apply a complex matrix to a complex vector.
 * The matrix is assumed to be square and the dimension of the
 * vector space is assumed to be a power of 2.
 * */
void mat_apply(const runtime::math::Unitary& mat,
               const runtime::math::Vector& vec,
               runtime::math::Vector& res);
// defined in mat_apply.S
extern "C" void mat_apply__avx(const void* mat, const void* vec, void* res, int dim);

namespace runtime {
namespace math {

Unitary Unitary::operator*(const Unitary& other) const {
#ifdef USE_SIMD__
    return mat_mul__simd(*this, other);
#else
    assert(this->dim() == other.dim());
    auto res = unitary_t::zero(this->dim());
    for (int i = 0; i < res.dim(); i++) {
        for (int j = 0; j < res.dim(); j++) {
            for (int k = 0; k < res.dim(); k++) {
                res(i, j) += (*this)(i, k)*other(k, j);
            }
        }
    }
    return res;
#endif
}

Vector Unitary::operator*(const Vector& target) const {
    assert(this->dim() == target.size());
    Vector res(target.size());
#ifdef USE_SIMD
    if (__builtin_cpu_supports("avx") && target.size() >= 4) {
        mat_apply__avx(this->_entries, target.ptr(), res.ptr(), target.size());
    } else {
        mat_apply(*this, target, res);
    }
#else
    mat_apply(*this, target, res);
#endif
    return res;
}

}
}

void mat_apply(const runtime::math::Unitary& mat,
               const runtime::math::Vector& vec,
               runtime::math::Vector& res)
{
    for (int i = 0; i < vec.size(); i++) {
        for (int j = 0; j < vec.size(); j++) {
            res[i] += mat(i, j)*vec[j];
        }
    }
}
