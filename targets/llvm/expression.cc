#include "expression.h"

#include <cmath>

using namespace llvm;
using namespace qasm;

Value* target_llvm::code_gen (LLVMContext& context,
                              std::shared_ptr<ast::RealNumber> real_num) {
    return ConstantFP::get(context, APFloat(real_num->value()));
}

Value* target_llvm::code_gen (LLVMContext& context,
                              std::shared_ptr<ast::NonNegativeInteger> nn_int) {
    return ConstantInt::get(context, APInt(NON_NEGATIVE_INTEGER_BITS, nn_int->value()));
}

Value* target_llvm::code_gen (LLVMContext& context,
                            std::shared_ptr<ast::EspecialConstant> ec) {
    switch (ec->name) {
    case ast::EspecialConstant::Pi:
        return ConstantFP::get(context, APFloat(M_PI));
    default:
        return ConstantFP::get(context, APFloat(M_PI));
    }
}

void target_llvm::code_gen (LLVMContext& context, std::shared_ptr<ast::Variable>) {
}

void target_llvm::code_gen (LLVMContext& context,
                            std::shared_ptr<ast::MinusExpression> min) {
}

Value* target_llvm::code_gen (LLVMContext& context,
                            std::shared_ptr<ast::AdditionExpression> add) {
    auto left = code_gen(context, add->left);
    auto right = code_gen(context, add->right);
}

Value* target_llvm::code_gen (LLVMContext& context,
                            std::shared_ptr<ast::MultiplicationExpression> mult) {
}

Value* target_llvm::code_gen (LLVMContext& context,
                            std::shared_ptr<ast::SubtractionExpression> sub) {
}

Value* target_llvm::code_gen (LLVMContext& context,
                            std::shared_ptr<ast::DivisionExpression> div) {
}
