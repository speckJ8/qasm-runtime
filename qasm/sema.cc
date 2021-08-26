#include "sema.h"

void qasm::sema::declare_symbols (qasm::ast::Program& program) {
    for (auto&& stmt : program.statements) {
        try {
            stmt->declare_symbols();
        } catch (const SemanticError& err) {
            throw SemanticError(err.msg, err.line, err.column, program.filename);
        }
    }
}

void qasm::sema::verify (qasm::ast::Program& program) {
    for (auto&& stmt : program.statements) {
        try {
            stmt->verify();
        } catch (const SemanticError& err) {
            throw SemanticError(err.msg, err.line, err.column, program.filename);
        }
    }
}
