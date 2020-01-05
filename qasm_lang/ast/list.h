#ifndef __QASM_LANG__AST_LIST_H__
#define __QASM_LANG__AST_LIST_H__

#include <vector>
#include <string>
#include "expression.h"

namespace qasm_lang {

namespace ast {

    class IdentifierList {
    public:
        std::vector<std::string> id_list;
        IdentifierList (std::vector<std::string> il): id_list(il) {};
    };


    class MixedList {
    public:
        std::vector<Variable> mixed_list;
        MixedList (std::vector<Variable> ml): mixed_list(ml) {};
    };


    class ExpressionList {
    public:
        std::vector<Expression> expression_list;
        ExpressionList (std::vector<Expression> el): expression_list(el) {};
    };

};

};


#endif // __QASM_LANG__AST_LIST_H__
