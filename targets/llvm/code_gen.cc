#include "code_gen.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <memory>

void target_llvm::generate_code (qasm::ast::Program& program) {
    using namespace llvm;
    LLVMContext context;
    // IRBuilder<> builder(context);
    auto topModule = std::make_unique<Module>("__qasm_top_module__", context);
}
