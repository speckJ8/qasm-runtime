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
    int _size { 0 };
    cx_t* _entries { nullptr };

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

    ~Vector() {
        if (_entries != nullptr) {
            free(_entries);
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
