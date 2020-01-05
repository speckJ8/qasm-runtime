#include "qasm_lang/parser.h"
#include "qasm_lang/parser_error.h"

int main (int argc, char** argv) {
    using namespace qasm_lang;
    try {
        ast::Program program = qasm_lang::parser::parse("tests/ok_sample.qasm");
    } catch (qasm_lang::parser::ParserError& e) {
        std::cout << e.to_string() << std::endl;
    }
    return 0;
}
