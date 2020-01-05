#ifndef __QASM_LANG__PARSER_H__
#define __QASM_LANG__PARSER_H__


#include <iostream>
#include "lexer.h"
#include "parser_error.h"
#include "ast/ast.h"


namespace qasm_lang {

namespace parser {

    ast::Program parse (std::string filename);

};

};


#endif // __QASM_LANG__PARSER_H__
