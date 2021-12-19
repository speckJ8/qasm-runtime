#ifndef __QASM_LANG__PARSER_H__
#define __QASM_LANG__PARSER_H__

#include <memory>

#include "lexer.h"
#include "parser_error.h"
#include "ast/ast.h"

namespace qasm {
namespace parser {

ast::Program parse (std::string filename);

};
};


#endif // __QASM_LANG__PARSER_H__
