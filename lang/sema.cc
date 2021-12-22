#include "sema.hpp"

namespace lang {

void sema::declare_symbols(Program& program) {
    for (auto&& stmt : program.statements) {
        stmt->declare_symbols();
    }
}

void sema::verify(Program& program) {
    for (auto&& stmt : program.statements) {
        stmt->verify();
    }
}

}
