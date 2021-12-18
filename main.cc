#include "qasm/parser.h"
#include "qasm/parser_error.h"
#include "qasm/sema.h"
#include "qasm/sema_error.h"
#include "qasm/symbol_table.h"

int main (int argc, char** argv) {
    using namespace qasm;

    try {
        ast::Program program = parser::parse("tests/ok_sample.qasm");
        sema::declare_symbols(program);
        sema::verify(program);
        // symbol_table::dump();
    } catch (parser::ParserError& e) {
        std::cout << e.to_string() << std::endl;
    } catch (sema::SemanticError& e) {
        std::cout << e.to_string() << std::endl;
    }
    return 0;
}
