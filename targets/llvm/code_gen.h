#ifndef __TARGETS_LLVM__CODE_GEN_H__
#define __TARGETS_LLVM__CODE_GEN_H__

#include "qasm/ast/ast.h"
#include "expression.h"

namespace target_llvm {

void generate_code (qasm::ast::Program& program);

};

#endif // __TARGETS_LLVM__CODE_GEN_H__ 
