#ifndef __QASM_LANG__AST_PROGRAM_H__
#define __QASM_LANG__AST_PROGRAM_H__


#include <vector>
#include "statement.h"

namespace qasm_lang {

namespace ast {

    /**
     * AST root node
     * */
    class Program {
    public:
        float open_qasm_version;
        std::vector<Statement> statements;

        Program (): open_qasm_version(2.0) {};
    };

};

};

#endif // __QASM_LANG__AST_PROGRAM_H__
