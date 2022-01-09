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

#ifndef __RUNTIME__UNITARY_H__
#define __RUNTIME__UNITARY_H__

#include "types.hpp"
#include "vector.hpp"

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <initializer_list>
#include <iostream>
#include <iomanip>
#include <vector>

namespace runtime {
namespace math {

// row-major indexing
#define __INDEX_ENTRY(r, c, dim) r*dim + c

class Unitary {
private:
    size_t _dim { 0 };
    cx_t* _entries { nullptr };

public:
    class Row : public Vector {
    protected:
        Unitary& _mat;
        size_t _size;
        size_t _row_index;

    public:
        Row(Unitary& mat, size_t size, size_t row_index):
            _mat(mat), _size(size), _row_index(row_index) {};

        size_t size() const override {
            return _size;
        }

        cx_t& operator()(size_t col) override {
            return _mat(_row_index, col);
        }

        const cx_t& operator()(size_t col) const override {
            return _mat(_row_index, col);
        }

        cx_t& operator[](size_t col) override {
            return _mat(_row_index, col);
        }

        const cx_t& operator[](size_t col) const override {
            return _mat(_row_index, col);
        }

        friend Unitary;

        virtual cx_t* ptr() override {
            return &_mat._entries[_row_index*_size];
        }

        virtual const cx_t* ptr() const override {
            return &_mat._entries[_row_index*_size];
        }
    };

    class Col : public Vector {
    protected:
        Unitary& _mat;
        size_t _size;
        size_t _col_index;

    public:
        Col(Unitary& mat, size_t size, size_t col_index):
            _mat(mat), _size(size), _col_index(col_index) {};

        size_t size() const override {
            return _size;
        }

        cx_t& operator()(size_t row) override {
            return _mat(row, _col_index);
        }

        const cx_t& operator()(size_t row) const override {
            return _mat(row, _col_index);
        }

        cx_t& operator[](size_t row) override {
            return _mat(row, _col_index);
        }

        const cx_t& operator[](size_t row) const override {
            return _mat(row, _col_index);
        }

        friend Unitary;
    };

    Unitary() = delete;
    Unitary(const Unitary&) = delete;
    Unitary operator=(const Unitary&) = delete;

    Unitary& operator=(const Unitary&& u) {
        _dim = u._dim;
        _entries = u._entries;
        return *this;
    }
    Unitary(const Unitary&& u): _dim(u._dim), _entries(u._entries) {}

    Unitary(size_t dim): _dim(dim) {
#ifdef USE_SIMD
        // allocate at a 32-byte address for use with AVX instructions
        _entries = static_cast<cx_t*>(aligned_alloc(32, _dim*_dim*sizeof(cx_t)));
#else
        _entries = static_cast<cx_t*>(malloc(_dim*_dim*sizeof(cx_t)));
#endif
        if (!_entries) {
            std::cout << "failed malloc: " << std::strerror(errno) << "\n";
            std::exit(EXIT_FAILURE);
        }
        for (size_t i = 0; i < _dim*_dim; i++) {
            _entries[i] = 0.f;
        }
    }

    Unitary(std::initializer_list<cx_t> entries) {
        size_t dim = std::floor(std::sqrt(entries.size()));
        assert(dim*dim == entries.size());
        _dim = dim;
#ifdef USE_SIMD
        // allocate at a 32-byte address for use with AVX instructions
        _entries = static_cast<cx_t*>(aligned_alloc(32, _dim*_dim*sizeof(cx_t)));
#else
        _entries = static_cast<cx_t*>(malloc(_dim*_dim*sizeof(cx_t)));
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

    ~Unitary() {
        if (_entries != nullptr) {
            free(_entries);
        }
    }

    size_t dim() const {
        return _dim;
    }

    size_t size() const {
        return _dim*_dim;
    }

    cx_t* ptr() {
        return _entries;
    }

    const cx_t* ptr() const {
        return _entries;
    }

    Row row(size_t index) {
        return Row(*this, _dim, index);
    }

    Col col(size_t index) {
        return Col(*this, _dim, index);
    }

    cx_t& operator()(size_t row, size_t col) {
        return _entries[__INDEX_ENTRY(row, col, _dim)];
    }

    const cx_t& operator()(size_t row, size_t col) const {
        return _entries[__INDEX_ENTRY(row, col, _dim)];
    }

    Unitary operator*(const Unitary& other) const;

    Vector operator*(const Vector& target) const;

    void redimension(size_t dimension, std::vector<size_t> targets, Unitary& res);

    friend std::ostream& operator<<(std::ostream& os, const Unitary& m) {
        for (size_t i = 0; i < m._dim; i++) {
            os << "|";
            for (size_t j = 0; j < m._dim; j++) {
                os << std::setw(3) << m(i, j);
            }
            os << " |" << std::endl;
        }
        return os;
    }
};

typedef struct Unitary unitary_t;

}
}

#endif // __RUNTIME__UNITARY_H__
