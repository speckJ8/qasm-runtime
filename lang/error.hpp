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
        if (context.filename != "") {
            os << "\u001b[31m" << "===== " << context.filename << "\u001b[0m";
        }
        os << "\u001b[31m\u001b[1m" << "===== Error: " << "\u001b[0m";
        os << "\u001b[1m"  << msg << "\u001b[0m\n";
        os << "      |\n";
        int num_lines = context.start_line - context.end_line;
        for (int i = 0; i <= num_lines; i++) {
            os << std::setw(5) << context.start_line + i << " |      ";
            os << input.get_read_line(context.start_line + i) << "\n";
        }
        auto spaces = std::string((context.start_col + 5), ' ');
        auto error_marker = std::string(context.end_col - context.start_col, '~');
        os << "      |" << spaces << "\u001b[1m" << error_marker << "\u001b[0m" << "\n";
        os << "      |\n";
        // return os;
    }
};
}

#endif //  __QASM_LANG__ERROR_H__
