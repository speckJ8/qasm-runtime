/**
 * Copyright (c) 2022 Alcides Andrade <junioralcides1994@gmail.com>
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

#include <iostream>
#include <ostream>
#include "lang/parser.hpp"
#include "lang/sema.hpp"
#include "lang/error.hpp"
#include "lang/program.hpp"
#include "lang/symbol_table.hpp"
// #include "runtime/runtime.hpp"

int main() {
    using namespace lang;
    std::string file = "sample.qasm";
    std::fstream fs(file);
    Input input(fs, file);
    try {
        auto program = parser::parse(input);
        sema::verify(program);
        // symbol_table::dump();
        // runtime::execute(program);
    } catch (Error& e) {
        e.show(std::cout, input);
    }
    fs.close();
    return 0;
}
