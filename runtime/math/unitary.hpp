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
#include <cstdlib>
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
    int _dim { 0 };
    cx_t* _entries { nullptr };

public:
    class Row : public Vector {
    protected:
        Unitary& _mat;
        int _size;
        int _row_index;

    public:
        Row(Unitary& mat, int size, int row_index):
            _mat(mat), _size(size), _row_index(row_index) {};

        int size() const override {
            return _size;
        }

        cx_t& operator()(int col) override {
            return _mat(_row_index, col);
        }

        const cx_t& operator()(int col) const override {
            return _mat(_row_index, col);
        }

        cx_t& operator[](int col) override {
            return _mat(_row_index, col);
        }

        const cx_t& operator[](int col) const override {
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
        int _size;
        int _col_index;

    public:
        Col(Unitary& mat, int size, int col_index):
            _mat(mat), _size(size), _col_index(col_index) {};

        int size() const override {
            return _size;
        }

        cx_t& operator()(int row) override {
            return _mat(row, _col_index);
        }

        const cx_t& operator()(int row) const override {
            return _mat(row, _col_index);
        }

        cx_t& operator[](int row) override {
            return _mat(row, _col_index);
        }

        const cx_t& operator[](int row) const override {
            return _mat(row, _col_index);
        }

        friend Unitary;
    };

    Unitary(int dim, std::vector<cx_t> entries): _dim(dim) {
        assert(_dim*_dim == (int)entries.size());
#ifdef USE_SIMD
        // allocate at a 32-byte address for use with AVX instructions
        _entries = static_cast<cx_t*>(aligned_alloc(32, _dim*_dim*sizeof(cx_t)));
#else
        _entries = static_cast<cx_t*>(malloc(_dim*_dim*sizeof(cx_t)));
#endif
        int i = 0;
        for (auto& cx : entries) {
            _entries[i++] = cx;
        }
    }

    Unitary(int dim, std::initializer_list<cx_t> entries): _dim(dim) {
        assert(_dim*_dim == (int)entries.size());
#ifdef USE_SIMD
        // allocate at a 32-byte address for use with AVX instructions
        _entries = static_cast<cx_t*>(aligned_alloc(32, _dim*_dim*sizeof(cx_t)));
#else
        _entries = static_cast<cx_t*>(malloc(_dim*_dim*sizeof(cx_t)));
#endif
        int i = 0;
        for (auto& cx : entries) {
            _entries[i++] = cx;
        }
    }

    ~Unitary() {
        if (_entries != nullptr) {
            free(_entries);
        }
    }

    static Unitary zero(int dim) {
        return { dim, std::vector<cx_t>(dim*dim, 0) };
    }

    int dim() const {
        return _dim;
    }

    int size() const {
        return _dim*_dim;
    }

    cx_t* ptr() {
        return _entries;
    }

    const cx_t* ptr() const {
        return _entries;
    }

    Row row(int index) {
        return Row(*this, _dim, index);
    }

    Col col(int index) {
        return Col(*this, _dim, index);
    }

    cx_t& operator()(int row, int col) {
        return _entries[__INDEX_ENTRY(row, col, _dim)];
    }

    const cx_t& operator()(int row, int col) const {
        return _entries[__INDEX_ENTRY(row, col, _dim)];
    }

    Unitary operator*(const Unitary& other) const;

    Vector operator*(const Vector& target) const;

    Unitary redimension(int dimension, std::vector<unsigned int> targets);

    friend std::ostream& operator<<(std::ostream& os, const Unitary& m) {
        for (int i = 0; i < m._dim; i++) {
            os << "|";
            for (int j = 0; j < m._dim; j++) {
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
