#ifndef __QASM_LANG__PARSER_H__
#define __QASM_LANG__PARSER_H__


#include <iostream>

#include "ast/node.h"


namespace qasm_lang {

namespace parser {

    /**
     * @param source_name qasm input stream
     * */
    void parse (std::istream& input);

};

};


#endif // __QASM_LANG__PARSER_H__
