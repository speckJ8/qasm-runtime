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

#ifndef __QASM_LANG__ERROR_H__
#define __QASM_LANG__ERROR_H__

#include "context.hpp"
#include "input.hpp"

#include <ostream>
#include <iomanip>

namespace lang {

class Error: public std::exception {
private:
    std::string _what;

public:
    Context context;
    std::string msg;

    Error(Context ctx, std::string msg): context(ctx), msg(msg) {
        _what = "[" + context.filename + ":" +
            std::to_string(context.start_line) + ":" +
            std::to_string(context.start_col) + "] " + msg;
    }

    const char* what() const throw() {
        return _what.c_str();
    }

    void show(std::ostream& os, Input& input) {
        os << "\u001b[31m\u001b[1m" << "===== Error: " << "\u001b[0m";
        os << "\u001b[1m"  << msg << "\u001b[0m\n";
        if (context.filename != "") {
            os << "\u001b[31m\u001b[1m" << "===== "  << "\u001b[0m" << context.filename << "\n";
        }
        os << "      |\n";
        int num_lines = context.end_line - context.start_line + 1;
        for (int i = 0; i < num_lines; i++) {
            auto line = input.get_read_line(context.start_line + i);
            os << std::setw(5) << context.start_line + i << " | ";
            os << line << "\n";
            auto start = i == 0 ? context.start_col : line.find_first_not_of(' ');
            auto end   = i == num_lines - 1 ? context.end_col : line.length();
            auto spaces = std::string(start, ' ');
            auto error_marker = std::string(end - start, '~');
            os << "      |" << spaces
               << "\u001b[31m\u001b[1m" << error_marker << "\u001b[0m\n";
        }
        if (context.end_line + 1 < input.number_of_lines_read()) {
            os << std::setw(5) << context.end_line + 1 << " | ";
            os << input.get_read_line(context.end_line + 1) << "\n";
        }
    }
};
}

#endif //  __QASM_LANG__ERROR_H__
