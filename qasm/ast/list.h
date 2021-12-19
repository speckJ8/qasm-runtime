#ifndef __QASM_LANG__AST_LIST_H__
#define __QASM_LANG__AST_LIST_H__

#include <vector>
#include <string>
#include <memory>

#include "expression.h"
#include "context.h"

namespace qasm {
namespace ast {

class IdentifierList {
public:
    Context context;
    std::vector<std::string> id_list;

    IdentifierList(std::vector<std::string> il, Context c):
        context(c), id_list(il)
    {};
};

class MixedList {
public:
    Context context;
    std::vector<Variable> mixed_list;

    MixedList(std::vector<Variable> ml, Context c):
        context(c), mixed_list(ml)
    {};
};

class ExpressionList {
public:
    Context context;
    std::vector<std::shared_ptr<Expression>> expression_list;

    ExpressionList(std::vector<std::shared_ptr<Expression>> el, Context c):
        context(c),
        expression_list(std::move(el))
    {};
};

};
};

#endif // __QASM_LANG__AST_LIST_H__
