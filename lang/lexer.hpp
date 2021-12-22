#ifndef __QASM_LANG__LEXER_H__
#define __QASM_LANG__LEXER_H__

#include "input.hpp"

#include <istream>
#include <variant>


namespace lang {
namespace lexer {

class Token {
public:
    enum Type {
        Id,
        Real,
        NonNegativeInteger,
        String,
        Keyword_OpenQasm,
        Keyword_Include,
        Keyword_Opaque,
        Keyword_If,
        Keyword_Barrier,
        Keyword_Qreg,
        Keyword_Creg,
        Keyword_Gate,
        Keyword_Measure,
        Keyword_Reset,
        Keyword_U,
        Keyword_CX,
        Keyword_Pi,
        Keyword_Sin,
        Keyword_Cos,
        Keyword_Tan,
        Keyword_Exp,
        Keyword_Ln,
        Keyword_Sqrt,
        Symbol_Semicolon,
        Symbol_Comma,
        Symbol_EqEq,
        Symbol_RightArrow,
        Symbol_LeftBracket,
        Symbol_RightBracket,
        Symbol_LeftParen,
        Symbol_RightParen,
        Symbol_LeftSquareParen,
        Symbol_RightSquareParen,
        Symbol_Plus,
        Symbol_Minus,
        Symbol_Times,
        Symbol_Divide,
        Symbol_Exp,

        Comment,
        Eof, 
        Invalid,
    };

    enum Kind {
        Identifier,
        Literal, // real, non negative interger or string
        Keyword,
        Symbol,

        CommentK,
        EofK,
        InvalidK,
    };

    static const std::string OpenQasm;
    static const std::string Include;
    static const std::string Opaque;
    static const std::string If;
    static const std::string Barrier;
    static const std::string Qreg;
    static const std::string Creg;
    static const std::string Gate;
    static const std::string Measure;
    static const std::string Reset;
    static const std::string U;
    static const std::string CX;
    static const std::string Pi;
    static const std::string Cos;
    static const std::string Sin;
    static const std::string Tan;
    static const std::string Exp;
    static const std::string Ln;
    static const std::string Sqrt;

    Type type;
    Kind kind;
    std::string value;
    unsigned int line, col;

    Token(Type type, Kind kind, std::string value, unsigned int line, unsigned int col):
        type(type), kind(kind), value(value), line(line), col(col) {};
};

/**
 * Return the string value for `type` if possible, i.e,
 * if `type` is of keyword or symbol kinds.
 * */
std::string repr(Token::Type type);


/**
 * Return the next lexical token.
 * */
Token next_token(Input& input);
Token _next_token(Input& input);

/**
 * Return a lookahead token.
 * */
void pushback_token(Token tk);

}
}

#endif // __QASM_LANG__LEXER_H__