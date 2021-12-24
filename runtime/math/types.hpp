#ifndef __RUNTIME__MATH_TYPES_H__
#define __RUNTIME__MATH_TYPES_H__

#include <complex>
#include <vector>

namespace runtime {
namespace math {

using namespace std::complex_literals;
using cx_t = std::complex<double>;
using unitary2d_t = cx_t[4];
using unitary4d_t = cx_t[16];
using unitary_t = std::vector<cx_t>;
using vector_t = std::vector<cx_t>;

}
}


#endif // __RUNTIME__MATH_TYPES_H__
