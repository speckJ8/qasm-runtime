#ifndef __QASM_LANG__CONTEXT_H__
#define __QASM_LANG__CONTEXT_H__

#include <string>

namespace qasm {

namespace ast {

/**
 * Information about where an AST node was parsed
 * */
struct Context {
public:
    unsigned int line;
    unsigned int column;
    std::string filename;

    Context(unsigned int l, unsigned int c, std::string f):
        line(l), column(c), filename(f)
    {};

    Context(unsigned int l, unsigned int c):
        line(l), column(c)
    {};
};

};

};

#endif // __QASM_LANG__CONTEXT_H__
