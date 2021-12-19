#include "sema.h"

namespace qasm {

void sema::declare_symbols(ast::Program& program) {
    for (auto&& stmt : program.statements) {
        try {
            stmt->declare_symbols();
        } catch (const SemanticError& err) {
            throw SemanticError(err.msg, err.line, err.column, program.filename);
        }
    }
}

void sema::verify(ast::Program& program) {
    for (auto&& stmt : program.statements) {
        try {
            stmt->verify();
        } catch (const SemanticError& err) {
            throw SemanticError(err.msg, err.line, err.column, program.filename);
        }
    }
}

}
