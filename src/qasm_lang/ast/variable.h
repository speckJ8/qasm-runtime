#ifndef __QASM_LANG__AST_VARIABLE_H__
#define __QASM_LANG__AST_VARIABLE_H__

#include <optional>
#include <vector>

#include "expression.h"

namespace qasm_lang {

namespace ast {
    
    class Variable: Expression {
    private:
        std::string identifier;
        // in case the variable is a vector and we're indexing it
        std::optional<unsigned int> index;

    public:
        Variable (std::string i): identifier(i), index({}) {};
        Variable (std::string i, unsigned int _i): identifier(i), index(_i) {};

        inline std::string name () { return identifier; };
    };

};

};

#endif // __QASM_LANG__AST_VARIABLE_H__
