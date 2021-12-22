#ifndef __QASM_LANG__CONTEXT_H__
#define __QASM_LANG__CONTEXT_H__

#include <string>
#include <vector>

namespace lang {

/**
 * Information about where an AST node was parsed
 * */
struct Context {
public:
    unsigned int start_line;
    unsigned int end_line;
    unsigned int start_col;
    unsigned int end_col;
    std::string filename;

    Context(unsigned int sln, unsigned int eln, unsigned int scn, unsigned int ecn, std::string f):
        start_line(sln),
        end_line(eln),
        start_col(scn),
        end_col(ecn),
        filename(f)
    {};
};

}

#endif // __QASM_LANG__CONTEXT_H__
