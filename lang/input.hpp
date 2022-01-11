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

#ifndef __QASM_LANG__INPUT_H__
#define __QASM_LANG__INPUT_H__

#include "context.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <optional>
#include <vector>


namespace lang {

class Input {
public:
    Input(std::istream& stream): _stream(stream) {};
    Input(std::istream& stream, std::string filename):
        _filename(filename), _stream(stream){};

    char peek();
    char get();
    bool eof();
    size_t line();
    size_t col();
    std::string filename();
    void strip_spaces();
    std::string get_read_line(size_t line);
    size_t number_of_lines_read();
    Context context(size_t start_line, size_t start_col);

private:
    size_t _line { 1 };
    size_t _col { 1 };
    std::string _filename { "" };
    std::vector<std::string> _lines_read;
    std::string _curr_line { "" };
    std::istream& _stream;
};

}

#endif // __QASM_LANG__INPUT_H__
