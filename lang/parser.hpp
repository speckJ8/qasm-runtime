#ifndef __QASM_LANG__PARSER_H__
#define __QASM_LANG__PARSER_H__

#include "input.hpp"
#include "lexer.hpp"
#include "program.hpp"

#include <memory>
#include <iostream>

namespace lang {
namespace parser {

Program parse(Input& input);

}
}


#endif // __QASM_LANG__PARSER_H__
