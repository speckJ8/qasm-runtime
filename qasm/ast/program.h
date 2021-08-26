#ifndef __QASM_LANG__AST_PROGRAM_H__
#define __QASM_LANG__AST_PROGRAM_H__


#include <vector>
#include <memory>

#include "statement.h"

namespace qasm {

namespace ast {

    /**
     * AST root node
     * */
    class Program {
    public:
        std::string filename;
        std::vector<std::shared_ptr<Statement>> statements;
        std::vector<Program> includes;

        Program (std::string filename, std::vector<std::shared_ptr<Statement>> s,
                 std::vector<Program> i):
            filename(filename), statements(std::move(s)), includes(std::move(i))
        {};
    };

};

};

#endif // __QASM_LANG__AST_PROGRAM_H__
