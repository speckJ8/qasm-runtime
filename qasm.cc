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
