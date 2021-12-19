#include <gtest/gtest.h>
#include <iostream>
#include <tuple>
#include "qasm/lexer.h"

TEST(Lexer, Basic) {
    using namespace qasm::lexer;

    std::istringstream input(
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

    for (int t = 0; t < tokens.size(); t++) {
        auto token = tokens[t];
        Token tk = next_token(input);
        EXPECT_EQ(tk.type, std::get<0>(token))
                << "[" << t << "]"
                << "expected value: " << std::get<2>(token) << "\n"
                << "[" << t << "]"
                << "actual value: " << tk.value;
        EXPECT_EQ(tk.kind, std::get<1>(token))
                << "[" << t << "]"
                << "expected value: " << std::get<2>(token) << "\n"
                << "[" << t << "]"
                << "actual value: " << tk.value;
        EXPECT_EQ(tk.value, std::get<2>(token));
    }
    Token tk = next_token(input);
    EXPECT_EQ(tk.type, Token::Eof);
    EXPECT_EQ(tk.kind, Token::EofK);
}
