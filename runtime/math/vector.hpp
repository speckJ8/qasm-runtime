#ifndef __RUNTIME__VECTOR_H__
#define __RUNTIME__VECTOR_H__

#include "types.hpp"

#include <cstdlib>
#include <cmath>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <vector>

namespace runtime {
namespace math {

class Vector {
private:
    int _size {0};
    cx_t* _entries;

public:
    Vector();
    Vector(int size): _size(size) {
#ifdef USE_SIMD
        // allocate at a 32-byte address for use with AVX instructions
        _entries = static_cast<cx_t*>(aligned_alloc(32, _size*sizeof(cx_t)));
#else
        _entries = static_cast<cx_t*>(malloc(_size*sizeof(cx_t)));
#endif
    };

    Vector(std::initializer_list<cx_t> entries): _size(entries.size()) {
#ifdef USE_SIMD
        // allocate at a 32-byte address for use with AVX instructions
        _entries = static_cast<cx_t*>(aligned_alloc(32, _size*sizeof(cx_t)));
#else
        _entries = static_cast<cx_t*>(malloc(_size*sizeof(cx_t)));
#endif
        int i = 0;
        for (auto& cx : entries) {
            _entries[i++] = cx;
        }
    }

    static Vector zero(int size) {
        Vector vec(size);
        for (int i = 0; i < size; i++) {
            vec[i] = 0;
        }
        return vec;
    }

    Vector tensor(const Vector& other) const;

    virtual int size() const {
        return _size;
    }

    virtual cx_t& operator()(int index) {
        return _entries[index];
    }
    virtual const cx_t& operator()(int index) const {
        return _entries[index];
    }

    virtual cx_t& operator[](int index) {
        return _entries[index];
    }
    virtual const cx_t& operator[](int index) const {
        return _entries[index];
    }

    const bool operator==(const Vector& other) const {
        for (int i = 0; i < size(); i++) {
            if (std::abs((*this)[i] - other[i]) > 0.001f) {
                return false;
            }
        }
        return true;
    }

    virtual cx_t* ptr() {
        return _entries;
    }

    virtual const cx_t* ptr() const {
        return _entries;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
        os << "{@" << v._size << " ";
        for (int i = 0; i < v._size; i++) {
            os << std::setw(3) << v._entries[i] << ", ";
        }
        os << " }" << std::endl;
        return os;
    }
};

typedef Vector vector_t;

}
}

#endif // __RUNTIME__VECTOR_H__
