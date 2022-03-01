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
#include "config.h"

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <vector>

namespace runtime {
namespace math {

class Vector {
private:
    size_t _size { 0 };
    cx_t* _entries { nullptr };

public:
    Vector() = delete;
    Vector(const Vector&) = delete;
    Vector operator=(const Vector&) = delete;

    Vector& operator=(Vector&& v) {
        if (_entries != nullptr) {
            free(_entries);
        }
        _size = v._size;
        _entries = v._entries;
        v._entries = nullptr;
        v._size = 0;
        return *this;
    }

    Vector(Vector&& v): _size(v._size), _entries(v._entries) {
        v._entries = nullptr;
    }

    Vector(size_t size): _size(size) {
#ifdef USE_SIMD
        // allocate at a 32-byte address for use with AVX instructions
        _entries = static_cast<cx_t*>(aligned_alloc(32, _size*sizeof(cx_t)));
#else
        _entries = static_cast<cx_t*>(malloc(_size*sizeof(cx_t)));
#endif
        if (!_entries) {
            std::cout << "failed malloc: " << std::strerror(errno) << "\n";
            std::exit(EXIT_FAILURE);
        }
        for (size_t i = 0; i < size; i++) {
            _entries[i] = 0.f;
        }
    };

    Vector(std::initializer_list<cx_t> entries): _size(entries.size()) {
#ifdef USE_SIMD
        // allocate at a 32-byte address for use with AVX instructions
        _entries = static_cast<cx_t*>(aligned_alloc(32, _size*sizeof(cx_t)));
#else
        _entries = static_cast<cx_t*>(malloc(_size*sizeof(cx_t)));
#endif
        if (!_entries) {
            std::cout << "failed malloc: " << std::strerror(errno) << "\n";
            std::exit(EXIT_FAILURE);
        }
        size_t i = 0;
        for (auto& cx : entries) {
            _entries[i++] = cx;
        }
    }

    ~Vector() {
        if (_entries != nullptr) {
            free(_entries);
        }
    }

    Vector tensor(const Vector& other) const;

    inline size_t size() const {
        return _size;
    }

    inline cx_t& operator()(size_t index) {
        return _entries[index];
    }

    inline const cx_t& operator()(size_t index) const {
        return _entries[index];
    }

    inline cx_t& operator[](size_t index) {
        return _entries[index];
    }

    inline const cx_t& operator[](size_t index) const {
        return _entries[index];
    }

    inline cx_t* ptr() {
        return _entries;
    }

    inline const cx_t* ptr() const {
        return _entries;
    }

    void reset(size_t, size_t);

    void measure(std::vector<bool>&);
    void measure(size_t, size_t, std::vector<bool>&);

    /**
     * Normalize the vector such that the sum of the square of the
     * absolute values of the coefficients equals 1.
     * */
    void normalize();


    friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
        os << "{ ";
        for (size_t i = 0; i < v._size; i++) {
            os << std::setw(3) << v._entries[i] << ", ";
        }
        os << " }";
        return os;
    }

    bool operator==(const Vector& other) const {
        for (size_t i = 0; i < size(); i++) {
            if (std::abs((*this)[i] - other[i]) > 0.001f) {
                return false;
            }
        }
        return true;
    }
};

typedef Vector vector_t;

}
}

#endif // __RUNTIME__VECTOR_H__
