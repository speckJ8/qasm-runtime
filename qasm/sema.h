#ifndef __QASM_LANG__SEMMA_H__
#define __QASM_LANG__SEMMA_H__

#include "ast/ast.h"
#include "sema_error.h"
#include "symbol_table.h"

#include <iostream>

namespace qasm {

namespace sema {

void declare_symbols(ast::Program& program);
void verify(ast::Program& program);

};

};


#endif // __QASM_LANG__SEMMA_H__
