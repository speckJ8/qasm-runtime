#ifndef __RUNTIME__MATH_SIMD_H__
#define __RUNTIME__MATH_SIMD_H__

#include "unitary.hpp"
#include "vector.hpp"
#include <immintrin.h>

namespace runtime {
namespace math {

void mat_apply__simd(const cx_t* mat, const cx_t* vec, cx_t* res, int dim);
unitary_t mat_mul__simd(unitary_t mat_a, unitary_t mat_b);

}
}

#endif // __RUNTIME__MATH_SIMD_H__
