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

int main (int argc, char** argv) {
    using namespace lang;
    std::istringstream ss(
        "OPENQASM 2.0;\n"
        "qreg qq[2];\n"
        "creg cc[2];\n"
        "gate cx c,t { CX c,t; }\n"
        "# this is some comment... \n"
        "gate x a { u3(pi,2/3,0) a; }\n"
        "gate y a { U(0,0,2*3^4.0) a; }\n"
        "# this is another comment\n"
        "# test, test, test\n"
        "x qq[0];\n"
        "y qq[1];\n"
        "barrier qq;\n"
        "measure qq -> cc;\n"
        "reset qq;"
    );
    Input input(ss);

    try {
        Program program = parser::parse(input);
        sema::declare_symbols(program);
        sema::verify(program);
        symbol_table::dump();
    } catch (Error& e) {
        e.show(std::cout, input);
    }
    return 0;
}
