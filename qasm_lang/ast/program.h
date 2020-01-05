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
        std::string filename;
        std::vector<Statement> statements;
        std::vector<Program> includes;

        Program (std::string filename, std::vector<Statement> s, std::vector<Program> i):
            filename(filename), statements(s), includes(i)
        {};
    };

};

};

#endif // __QASM_LANG__AST_PROGRAM_H__
