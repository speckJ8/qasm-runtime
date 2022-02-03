/**
 * Copyright (c) 2022 Alcides Andrade <andrade.alcides.junior@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __QASM_LANG__LEXER_H__
#define __QASM_LANG__LEXER_H__

#include <istream>
#include <tuple>
#include <variant>

#include "input.hpp"

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
    size_t line, col;

    Token(Type type, Kind kind, std::string value, size_t line, size_t col):
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
