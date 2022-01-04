#include "vector.hpp"

namespace runtime {
namespace math {

Vector Vector::tensor(const Vector& other) const {
    Vector res(this->size()*other.size());
    for (int i = 0; i < this->size(); i++) {
        for (int j = 0; j < other.size(); j++) {
            res[i*other.size() + j] = (*this)[i]*other[j];
        }
    }
    return res;
}

}
}
