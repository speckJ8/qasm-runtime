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

#include <gtest/gtest.h>
#include <iostream>
#include <tuple>
#include "lang/lexer.hpp"

TEST(Lexer, Basic) {
    using namespace lang;
    using namespace lang::lexer;

    std::istringstream ss(
        "OPENQASM 2.0;"
        "qreg qq[2];"
        "creg cc[2];"
        "gate cx c,t { CX c,t; }"
        "# this is some comment... \n"
        "gate x a { u3(pi,2/3,0) a; }"
        "gate y a { U(0,0,2*3^4.0) a; }"
        "# this is another comment\n"
        "# test, test, test\n"
        "x qq[0];"
        "y qq[1];"
        "barrier qq;"
        "measure qq -> cc;"
        "reset qq;"
    );
    Input input(ss);
    std::vector<std::tuple<Token::Type, Token::Kind, std::string>> tokens = {
        // OPENQASM 2.0;
        {
            Token::Keyword_OpenQasm,
            Token::Keyword,
            "OPENQASM",
        },
        {
            Token::Real,
            Token::Literal,
            "2.0",
        },
        {
            Token::Symbol_Semicolon,
            Token::Symbol,
            ";",
        },
        // qreg qq[2];
        {
            Token::Keyword_Qreg,
            Token::Keyword,
            "qreg",
        },
        {
            Token::Id,
            Token::Identifier,
            "qq",
        },
        {
            Token::Symbol_LeftSquareParen,
            Token::Symbol,
            "[",
        },
        {
            Token::NonNegativeInteger,
            Token::Literal,
            "2",
        },
        {
            Token::Symbol_RightSquareParen,
            Token::Symbol,
            "]",
        },
        {
            Token::Symbol_Semicolon,
            Token::Symbol,
            ";",
        },
        // creg cc[2];
        {
            Token::Keyword_Creg,
            Token::Keyword,
            "creg",
        },
        {
            Token::Id,
            Token::Identifier,
            "cc",
        },
        {
            Token::Symbol_LeftSquareParen,
            Token::Symbol,
            "[",
        },
        {
            Token::NonNegativeInteger,
            Token::Literal,
            "2",
        },
        {
            Token::Symbol_RightSquareParen,
            Token::Symbol,
            "]",
        },
        {
            Token::Symbol_Semicolon,
            Token::Symbol,
            ";",
        },
        // gate cx c,t { CX c,t; }
        {
            Token::Keyword_Gate,
            Token::Keyword,
            "gate",
        },
        {
            Token::Id,
            Token::Identifier,
            "cx",
        },
        {
            Token::Id,
            Token::Identifier,
            "c",
        },
        {
            Token::Symbol_Comma,
            Token::Symbol,
            ",",
        },
        {
            Token::Id,
            Token::Identifier,
            "t",
        },
        {
            Token::Symbol_LeftBracket,
            Token::Symbol,
            "{",
        },
        {
            Token::Keyword_CX,
            Token::Keyword,
            "CX",
        },
        {
            Token::Id,
            Token::Identifier,
            "c",
        },
        {
            Token::Symbol_Comma,
            Token::Symbol,
            ",",
        },
        {
            Token::Id,
            Token::Identifier,
            "t",
        },
        {
            Token::Symbol_Semicolon,
            Token::Symbol,
            ";",
        },
        {
            Token::Symbol_RightBracket,
            Token::Symbol,
            "}",
        },
        {
            Token::Comment,
            Token::CommentK,
            "",
        },
        // gate x a { u3(pi,2/3,0) a; }
        {
            Token::Keyword_Gate,
            Token::Keyword,
            "gate",
        },
        {
            Token::Id,
            Token::Identifier,
            "x",
        },
        {
            Token::Id,
            Token::Identifier,
            "a",
        },
        {
            Token::Symbol_LeftBracket,
            Token::Symbol,
            "{",
        },
        {
            Token::Id,
            Token::Identifier,
            "u3",
        },
        {
            Token::Symbol_LeftParen,
            Token::Symbol,
            "(",
        },
        {
            Token::Keyword_Pi,
            Token::Keyword,
            "pi",
        },
        {
            Token::Symbol_Comma,
            Token::Symbol,
            ",",
        },
        {
            Token::NonNegativeInteger,
            Token::Literal,
            "2",
        },
        {
            Token::Symbol_Divide,
            Token::Symbol,
            "/",
        },
        {
            Token::NonNegativeInteger,
            Token::Literal,
            "3",
        },
        {
            Token::Symbol_Comma,
            Token::Symbol,
            ",",
        },
        {
            Token::NonNegativeInteger,
            Token::Literal,
            "0",
        },
        {
            Token::Symbol_RightParen,
            Token::Symbol,
            ")",
        },
        {
            Token::Id,
            Token::Identifier,
            "a",
        },
        {
            Token::Symbol_Semicolon,
            Token::Symbol,
            ";",
        },
        {
            Token::Symbol_RightBracket,
            Token::Symbol,
            "}",
        },
        // gate y a { U(0,0,2*3^4.0) a; }
        {
            Token::Keyword_Gate,
            Token::Keyword,
            "gate",
        },
        {
            Token::Id,
            Token::Identifier,
            "y",
        },
        {
            Token::Id,
            Token::Identifier,
            "a",
        },
        {
            Token::Symbol_LeftBracket,
            Token::Symbol,
            "{",
        },
        {
            Token::Keyword_U,
            Token::Keyword,
            "U",
        },
        {
            Token::Symbol_LeftParen,
            Token::Symbol,
            "(",
        },
        {
            Token::NonNegativeInteger,
            Token::Literal,
            "0",
        },
        {
            Token::Symbol_Comma,
            Token::Symbol,
            ",",
        },
        {
            Token::NonNegativeInteger,
            Token::Literal,
            "0",
        },
        {
            Token::Symbol_Comma,
            Token::Symbol,
            ",",
        },
        {
            Token::NonNegativeInteger,
            Token::Literal,
            "2",
        },
        {
            Token::Symbol_Times,
            Token::Symbol,
            "*",
        },
        {
            Token::NonNegativeInteger,
            Token::Literal,
            "3",
        },
        {
            Token::Symbol_Exp,
            Token::Symbol,
            "^",
        },
        {
            Token::Real,
            Token::Literal,
            "4.0",
        },
        {
            Token::Symbol_RightParen,
            Token::Symbol,
            ")",
        },
        {
            Token::Id,
            Token::Identifier,
            "a",
        },
        {
            Token::Symbol_Semicolon,
            Token::Symbol,
            ";",
        },
        {
            Token::Symbol_RightBracket,
            Token::Symbol,
            "}",
        },
        {
            Token::Comment,
            Token::CommentK,
            "",
        },
        {
            Token::Comment,
            Token::CommentK,
            "",
        },
        // x qq[0];
        {
            Token::Id,
            Token::Identifier,
            "x",
        },
        {
            Token::Id,
            Token::Identifier,
            "qq",
        },
        {
            Token::Symbol_LeftSquareParen,
            Token::Symbol,
            "[",
        },
        {
            Token::NonNegativeInteger,
            Token::Literal,
            "0",
        },
        {
            Token::Symbol_RightSquareParen,
            Token::Symbol,
            "]",
        },
        {
            Token::Symbol_Semicolon,
            Token::Symbol,
            ";",
        },
        // y qq[1];
        {
            Token::Id,
            Token::Identifier,
            "y",
        },
        {
            Token::Id,
            Token::Identifier,
            "qq",
        },
        {
            Token::Symbol_LeftSquareParen,
            Token::Symbol,
            "[",
        },
        {
            Token::NonNegativeInteger,
            Token::Literal,
            "1",
        },
        {
            Token::Symbol_RightSquareParen,
            Token::Symbol,
            "]",
        },
        {
            Token::Symbol_Semicolon,
            Token::Symbol,
            ";",
        },
        // barrier qq;
        {
            Token::Keyword_Barrier,
            Token::Keyword,
            "barrier",
        },
        {
            Token::Id,
            Token::Identifier,
            "qq",
        },
        {
            Token::Symbol_Semicolon,
            Token::Symbol,
            ";",
        },
        // measure qq -> cc;
        {
            Token::Keyword_Measure,
            Token::Keyword,
            "measure",
        },
        {
            Token::Id,
            Token::Identifier,
            "qq",
        },
        {
            Token::Symbol_RightArrow,
            Token::Symbol,
            "->",
        },
        {
            Token::Id,
            Token::Identifier,
            "cc",
        },
        {
            Token::Symbol_Semicolon,
            Token::Symbol,
            ";",
        },
        // reset qq;
        {
            Token::Keyword_Reset,
            Token::Keyword,
            "reset",
        },
        {
            Token::Id,
            Token::Identifier,
            "qq",
        },
        {
            Token::Symbol_Semicolon,
            Token::Symbol,
            ";",
        },
    };

    int i = 0;
    for (auto t = tokens.begin(); t != tokens.end(); t++, i++) {
        auto token = *t;
        Token tk = next_token(input);
        EXPECT_EQ(tk.type, std::get<0>(token))
                << "[" << i << "]"
                << "expected value: " << std::get<2>(token) << "\n"
                << "[" << i << "]"
                << "actual value: " << tk.value;
        EXPECT_EQ(tk.kind, std::get<1>(token))
                << "[" << i << "]"
                << "expected value: " << std::get<2>(token) << "\n"
                << "[" << i << "]"
                << "actual value: " << tk.value;
        EXPECT_EQ(tk.value, std::get<2>(token));
    }
    Token tk = next_token(input);
    EXPECT_EQ(tk.type, Token::Eof);
    EXPECT_EQ(tk.kind, Token::EofK);
}
