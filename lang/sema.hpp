#ifndef __QASM_LANG__SEMMA_H__
#define __QASM_LANG__SEMMA_H__

#include "program.hpp"

#include <iostream>

namespace lang {
namespace sema {

void declare_symbols(Program& program);
void verify(Program& program);

}
}


#endif // __QASM_LANG__SEMMA_H__
