#ifndef __QASM_LANG__PARSER_ERROR_H__
#define __QASM_LANG__PARSER_ERROR_H__

#include <string>
#include <exception>
#include <iostream>

namespace qasm_lang {

namespace parser {

    class ParserError : public std::exception {
    public:
        unsigned int line, column;
        std::string msg;

        ParserError (std::string msg, unsigned int line, unsigned int column):
            line(line), column(column), msg(msg) {};

        const char* what () const throw () {
            auto str = to_string();
            return str.c_str();
        }

        inline std::string to_string () const {
            std::string error_msg = "[parser-error:" + std::to_string(line) + ":" +
                                    std::to_string(column) + "] " + msg;
            return error_msg;
        }
    };

};

};


#endif // __QASM_LANG__PARSER_ERROR_H__
