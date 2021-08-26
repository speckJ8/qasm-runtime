#ifndef __TARGETS_LLVM__EXPRESSION_GEN_H__
#define __TARGETS_LLVM__EXPRESSION_GEN_H__

#include "qasm/ast/ast.h"
#include <memory>
#include <llvm/IR/LLVMContext.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/IR/Constants.h>

namespace target_llvm {

/**
 * Generate the IR for expression nodes
 * */

using namespace llvm;
using namespace qasm;

/**
 * Number of bits used for non negative integer values
 * */
const int NON_NEGATIVE_INTEGER_BITS = 64;

Value* code_gen (LLVMContext&, std::shared_ptr<ast::RealNumber>);
Value* code_gen (LLVMContext&, std::shared_ptr<ast::NonNegativeInteger>);
Value* code_gen (LLVMContext&, std::shared_ptr<ast::EspecialConstant>);
// void code_gen (LLVMContext&, std::shared_ptr<ast::Variable>);
void code_gen (LLVMContext&, std::shared_ptr<ast::MinusExpression>);
Value* code_gen (LLVMContext&, std::shared_ptr<ast::AdditionExpression>);
Value* code_gen (LLVMContext&, std::shared_ptr<ast::MultiplicationExpression>);
Value* code_gen (LLVMContext&, std::shared_ptr<ast::SubtractionExpression>);
Value* code_gen (LLVMContext&, std::shared_ptr<ast::DivisionExpression>);

};

#endif // __TARGETS_LLVM__EXPRESSION_GEN_H__ 
