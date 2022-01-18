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
extern "C" void mat_apply__avx(const void* mat, const void* vec, void* res, size_t dim);

/**
 * Multiply two square complex matrices. The matrix dimension is
 * assumed to be a power of 2.
 * */
void mat_mul(const runtime::math::Unitary& mat_a,
             const runtime::math::Unitary& mat_b,
             runtime::math::Unitary& res);
// defined in mat_mul.S
extern "C" void mat_mul__avx(const void* mat_a, const void* mat_b, void* res, size_t dim);

/**
 * Compute the tensor product of two complex matrices.
 * */
void mat_tensor(const runtime::math::Unitary& mat_a,
                const runtime::math::Unitary& mat_b,
                runtime::math::Unitary& res);

size_t permute_index(const std::vector<size_t>& permutation, size_t index);

namespace runtime {
namespace math {

Vector Unitary::operator*(const Vector& target) const {
    assert(this->dim() == target.size());
    Vector res(this->dim());
#ifdef USE_SIMD
    if (__builtin_cpu_supports("avx") && this->dim() >= 4) {
        mat_apply__avx(this->ptr(), target.ptr(), res.ptr(), target.size());
    } else {
        mat_apply(*this, target, res);
    }
#else
    mat_apply(*this, target, res);
#endif
    return res;
}

Unitary Unitary::operator*(const Unitary& other) const {
    assert(this->dim() == other.dim());
    Unitary res(this->dim());
#ifdef USE_SIMD
    if (__builtin_cpu_supports("avx") && this->dim() >= 4) {
        mat_mul__avx(this->ptr(), other.ptr(), res.ptr(), this->dim());
    } else {
        mat_mul(*this, other, res);
    }
#else
    mat_mul(*this, other, res);
#endif
    return res;
}

Unitary Unitary::redimension(const std::vector<size_t>& permutation) {
    size_t dim = std::exp2(permutation.size());
    Unitary id = Unitary::id(dim/this->dim());
    Unitary tn = this->tensor(id);
    Unitary res(tn.dim());
    for (size_t i = 0; i < res.dim(); i++) {
        for (size_t j = 0; j < res.dim(); j++) {
            auto k = permute_index(permutation, i);
            auto l = permute_index(permutation, j);
            std::cout << "p(" << i << ") = " << k << ", "
                      << "p(" << j << ") = " << l << "\n";
            res(k, l) = tn(i, j);
        }
    }
    return res;
}

Unitary Unitary::tensor(const Unitary& other) const {
    Unitary res(this->dim()*other.dim());
    mat_tensor(*this, other, res);
    return res;
}

}
}

void mat_apply(const runtime::math::Unitary& mat,
               const runtime::math::Vector& vec,
               runtime::math::Vector& res)
{
    for (size_t i = 0; i < vec.size(); i++) {
        for (size_t j = 0; j < vec.size(); j++) {
            res[i] += mat(i, j)*vec[j];
        }
    }
}

void mat_mul(const runtime::math::Unitary& mat_a,
             const runtime::math::Unitary& mat_b,
             runtime::math::Unitary& res)
{
    for (size_t i = 0; i < res.dim(); i++) {
        for (size_t k = 0; k < res.dim(); k++) {
            for (size_t j = 0; j < res.dim(); j++) {
                res(i, j) += mat_a(i, k)*mat_b(k, j);
            }
        }
    }
}

void mat_tensor(const runtime::math::Unitary& mat_a,
                const runtime::math::Unitary& mat_b,
                runtime::math::Unitary& res)
{
    for (size_t i = 0; i < mat_a.dim(); i++) {
        for (size_t k = 0; k < mat_b.dim(); k++) {
            for (size_t j = 0; j < mat_a.dim(); j++) {
                for (size_t l = 0; l < mat_b.dim(); l++) {
                    res(i*mat_b.dim() + k, j*mat_b.dim() + l) = mat_a(i, j)*mat_b(k, l);
                }
            }
        }
    }
}

size_t permute_index(const std::vector<size_t>& permutation, size_t index) {
    size_t new_index = 0;
    for (size_t i = 0; i < permutation.size(); i++) {
        size_t val = (1 << i) & index;
        if (permutation[i] > i) {
            val <<= (permutation[i] - i);
        } else {
            val >>= (i - permutation[i]);
        }
        new_index |= val;
    }
    return new_index;
}
