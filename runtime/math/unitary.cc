#include "unitary.hpp"
#include "simd.hpp"

namespace runtime {
namespace math {

Unitary Unitary::operator*(const Unitary& other) const {
#ifdef USE_SIMD
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
#ifdef USE_SIMD
    const cx_t* mat = this->_entries;
    const cx_t* vec = target.ptr();
    Vector res(target.size());
    cx_t* _res = res.ptr();
    mat_apply__simd(mat, vec, _res, target.size());
    return res;
#else
    assert(this->dim() == target.size());
    Vector res(target.size());
    for (int i = 0; i < target.size(); i++) {
        for (int j = 0; j < target.size(); j++) {
            res[i] += (*this)(i, j)*target[j];
        }
    }
    return res;
#endif
}

}
}
