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
extern "C" void mat_apply__avx(const void* mat,
                               const void* vec,
                               void* res,
                               int dim,
                               int num_simd_loops);

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
        int num_simd_loops = std::floor(target.size()/4);
        mat_apply__avx(this->_entries, target.ptr(), res.ptr(), target.size(), num_simd_loops);
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
