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

#ifndef __QASM_RUNTIME__ERROR_H__
#define __QASM_RUNTIME__ERROR_H__

#include <ostream>
#include <iomanip>

namespace runtime {

class Error: public std::exception {
private:
    std::string _what;

public:
    std::string msg;

    Error(std::string msg) : msg(msg) {
    }

    const char* what() const throw() {
        return _what.c_str();
    }

    void show(std::ostream& os) {
        os << "\u001b[31m\u001b[1m" << "===== Error: " << "\u001b[0m";
        os << "\u001b[1m" << msg << "\u001b[0m\n";
    }
};
}  // namespace runtime

#endif  //  __QASM_RUNTIME__ERROR_H__
