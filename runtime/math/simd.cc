#include "simd.hpp"
#include <immintrin.h>

#include <cmath>

/**
 * Defined in simd.s
 * */
extern "C" void _mat_apply__sse3(const void* mat,
                                 const void* vec,
                                 void* res,
                                 int dim,
                                 int num_simd_loops);

namespace runtime {
namespace math {

__attribute__((target("sse3")))
void mat_apply__simd(const cx_t* mat, const cx_t* vec, cx_t* res, int dim) {
    int num_simd_loops = std::floor(dim/2);
    _mat_apply__sse3(mat, vec, res, dim, num_simd_loops);
}

__attribute__((target("sse3")))
unitary_t mat_mul__simd(unitary_t mat_a, unitary_t mat_b) {
    auto m = unitary_t::zero(0);
    return m;
}

}
}
