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

#include "input.hpp"

#include <cassert>
#include <fstream>
#include <ios>
#include <sstream>
#include <iostream>

namespace lang {

char Input::peek() {
    return this->_stream.peek();
}

char Input::get() {
    char next = this->_stream.get();
    if (next == '\n') {
        this->line++;
        this->col = 1;
        this->_lines_read.push_back(std::move(this->_curr_line));
        this->_curr_line = "";
    } else {
        this->_curr_line.push_back(next);
        this->col++;
    }
    return next;
}

bool Input::eof() {
    return this->_stream.eof();
}

void Input::strip_spaces() {
    while (std::isspace(this->_stream.peek()) && !this->_stream.eof()) {
        char next = this->_stream.get();
        if (next == '\n' || next == '\r') {
            this->line++;
            this->col = 1;
            this->_lines_read.push_back(std::move(this->_curr_line));
            this->_curr_line = "";
        } else {
            this->col++;
            this->_curr_line.push_back(next);
        }
    }
}

Context Input::context(size_t start_line, size_t start_col) {
    return Context(start_line, this->line, start_col, this->col, this->filename);
}

std::string Input::get_read_line(size_t line) {
    assert(line < this->_lines_read.size());
    return this->_lines_read[line-1];
}

size_t Input::number_of_lines_read() {
    return this->_lines_read.size();
}

}
