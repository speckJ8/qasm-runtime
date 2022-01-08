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

#include "lexer.hpp"

#include <cctype>
#include <utility>
#include <sstream>
#include <optional>
#include <stack>
#include <iostream>

namespace lang {

std::stack<lexer::Token> _token_stack;

const std::string lexer::Token::OpenQasm = "OPENQASM";
const std::string lexer::Token::Include  = "include";
const std::string lexer::Token::Opaque   = "opaque";
const std::string lexer::Token::If       = "if";
const std::string lexer::Token::Barrier  = "barrier";
const std::string lexer::Token::Qreg     = "qreg";
const std::string lexer::Token::Creg     = "creg";
const std::string lexer::Token::Gate     = "gate";
const std::string lexer::Token::Measure  = "measure";
const std::string lexer::Token::Reset    = "reset";
const std::string lexer::Token::U        = "U";
const std::string lexer::Token::CX       = "CX";
const std::string lexer::Token::Pi       = "pi";
const std::string lexer::Token::Cos      = "cos";
const std::string lexer::Token::Sin      = "sin";
const std::string lexer::Token::Tan      = "tan";
const std::string lexer::Token::Exp      = "exp";
const std::string lexer::Token::Ln       = "ln";
const std::string lexer::Token::Sqrt     = "sqrt";


std::pair<std::string, bool> read_number(char first_char, Input& input);
std::string read_string(char first_char, Input& input);
std::optional<std::string> read_literal_string(Input& input);
std::string read_comment(Input& input);


std::string lexer::repr(lexer::Token::Type type) {
    switch (type) {
    case Token::Keyword_OpenQasm:
        return Token::OpenQasm;
    case Token::Keyword_Include:
        return Token::Include;
    case Token::Keyword_Opaque:
        return Token::Opaque;
    case Token::Keyword_If:
        return Token::If;
    case Token::Keyword_Barrier:
        return Token::Barrier;
    case Token::Keyword_Qreg:
        return Token::Qreg;
    case Token::Keyword_Creg:
        return Token::Creg;
    case Token::Keyword_Gate:
        return Token::Gate;
    case Token::Keyword_Measure:
        return Token::Measure;
    case Token::Keyword_Reset:
        return Token::Reset;
    case Token::Keyword_U:
        return Token::U;
    case Token::Keyword_CX:
        return Token::CX;
    case Token::Keyword_Pi:
        return Token::Pi;
    case Token::Keyword_Sin:
        return Token::Sin;
    case Token::Keyword_Cos:
        return Token::Cos;
    case Token::Keyword_Tan:
        return Token::Tan;
    case Token::Keyword_Exp:
        return Token::Exp;
    case Token::Keyword_Ln:
        return Token::Ln;
    case Token::Keyword_Sqrt:
        return Token::Sqrt;
    case Token::Symbol_Semicolon:
        return ";";
    case Token::Symbol_Comma:
        return ",";
    case Token::Symbol_EqEq:
        return "==";
    case Token::Symbol_LeftBracket:
        return "{";
    case Token::Symbol_RightArrow:
        return "->";
    case Token::Symbol_RightBracket:
        return "}";
    case Token::Symbol_LeftParen:
        return "(";
    case Token::Symbol_RightParen:
        return ")";
    case Token::Symbol_LeftSquareParen:
        return "[";
    case Token::Symbol_RightSquareParen:
        return "]";
    case Token::Symbol_Plus:
        return "+";
    case Token::Symbol_Minus:
        return "-";
    case Token::Symbol_Times:
        return "*";
    case Token::Symbol_Divide:
        return "/";
    case Token::Symbol_Exp:
        return "^";
    case Token::Id:
        return "<identifier>";
    case Token::Real:
        return "<real-number>";
    case Token::NonNegativeInteger:
        return "<non-negative-integer>";
    case Token::String:
        return "<string>";
    case Token::Eof:
        return "<EOF>";
    case Token::Invalid:
        return "<invalid>";
    case Token::Comment:
        return "<comment>";
    default:
        return "N/A";
    }
}

lexer::Token lexer::_next_token(Input& input) {
    if (!_token_stack.empty()) {
        auto tk = std::move(_token_stack.top());
        _token_stack.pop();
        return tk;
    }

    input.strip_spaces();
    unsigned int line = input.line;
    unsigned int col = input.col;
    char next = input.get();

    if (input.eof())
        return Token(Token::Eof, Token::EofK, "", line, col);
 
    switch (next) {
    case '{':
        return Token(Token::Symbol_LeftBracket, Token::Symbol, "{", line, col);
    case '}':
        return Token(Token::Symbol_RightBracket, Token::Symbol, "}", line, col);
    case ';':
        return Token(Token::Symbol_Semicolon, Token::Symbol, ";", line, col);
    case ',':
        return Token(Token::Symbol_Comma, Token::Symbol, ",", line, col);
    case '(':
        return Token(Token::Symbol_LeftParen, Token::Symbol, "(", line, col);
    case ')':
        return Token(Token::Symbol_RightParen, Token::Symbol, ")", line, col);
    case '[':
        return Token(Token::Symbol_LeftSquareParen, Token::Symbol, "[", line, col);
    case ']':
        return Token(Token::Symbol_RightSquareParen, Token::Symbol, "]", line, col);
    case '+':
        return Token(Token::Symbol_Plus, Token::Symbol, "+", line, col);
    case '-':
        if (input.peek() == '>') {
            input.get();
            return Token(Token::Symbol_RightArrow, Token::Symbol, "->", line, col);
        }
        return Token(Token::Symbol_Minus, Token::Symbol, "-", line, col);
    case '*':
        return Token(Token::Symbol_Times, Token::Symbol, "*", line, col);
    case '/':
        return Token(Token::Symbol_Divide, Token::Symbol, "/", line, col);
    case '^':
        return Token(Token::Symbol_Exp, Token::Symbol, "^", line, col);
    case '=':
        if (input.get() == '=') {
            return Token(Token::Symbol_EqEq, Token::Symbol, "==", line, col);
        } else {
            return Token(Token::Invalid, Token::InvalidK, "", line, col);
        }
    case '#':
        return Token(Token::Comment, Token::CommentK, read_comment(input), line, col);
    }

    if (std::isalpha(next) || next == '_') {
        std::string text = read_string(next, input);
        if (text == Token::OpenQasm)
            return Token(Token::Keyword_OpenQasm, Token::Keyword, text, line, col);
        if (text == Token::Include)
            return Token(Token::Keyword_Include, Token::Keyword, text, line, col);
        if (text == Token::Opaque)
            return Token(Token::Keyword_Opaque, Token::Keyword, text, line, col);
        if (text == Token::If)
            return Token(Token::Keyword_If, Token::Keyword, text, line, col);    
        if (text == Token::Barrier)
            return Token(Token::Keyword_Barrier, Token::Keyword, text, line, col);
        if (text == Token::Qreg)
            return Token(Token::Keyword_Qreg, Token::Keyword, text, line, col);
        if (text == Token::Creg)
            return Token(Token::Keyword_Creg, Token::Keyword, text, line, col);
        if (text == Token::Gate)
            return Token(Token::Keyword_Gate, Token::Keyword, text, line, col);
        if (text == Token::Measure)
            return Token(Token::Keyword_Measure, Token::Keyword, text, line, col);
        if (text == Token::Reset)
            return Token(Token::Keyword_Reset, Token::Keyword, text, line, col);
        if (text == Token::U)
            return Token(Token::Keyword_U, Token::Keyword, text, line, col);
        if (text == Token::CX)
            return Token(Token::Keyword_CX, Token::Keyword, text, line, col);
        if (text == Token::Pi)
            return Token(Token::Keyword_Pi, Token::Keyword, text, line, col);
        if (text == Token::Cos)
            return Token(Token::Keyword_Cos, Token::Keyword, text, line, col);
        if (text == Token::Sin)
            return Token(Token::Keyword_Cos, Token::Keyword, text, line, col);
        if (text == Token::Tan)
            return Token(Token::Keyword_Tan, Token::Keyword, text, line, col);
        if (text == Token::Exp)
            return Token(Token::Keyword_Exp, Token::Keyword, text, line, col);
        if (text == Token::Ln)
            return Token(Token::Keyword_Ln, Token::Keyword, text, line, col);
        if (text == Token::Sqrt)
            return Token(Token::Keyword_Sqrt, Token::Keyword, text, line, col);

        return Token(Token::Id, Token::Identifier, text, line, col);
    }
    else if (std::isdigit(next)) {
        auto [ num, isinteger ] = read_number(next, input);
        if (isinteger)
            return Token(Token::NonNegativeInteger, Token::Literal, num, line, col);
        else
            return Token(Token::Real, Token::Literal, num, line, col);
    }
    else if (next == '"') {
        auto str = read_literal_string(input);
        if (str) {
            return Token(Token::String, Token::Literal, str.value(), line, col);
        }
        else
            return Token(Token::Invalid, Token::InvalidK, "", line, col);
    }
    else {
        return Token(Token::Invalid, Token::InvalidK, std::string(1, next), line, col);
    }
}
lexer::Token lexer::next_token(Input& input) {
    auto tok = _next_token(input);
    // std::cout << "produced " << tok.value << "\n";
    return tok;
}

void lexer::pushback_token(lexer::Token tk) {
    // std::cout << "pushback " << tk.value << "\n";
    _token_stack.push(tk);
}


std::string read_string(char first_char, Input& input) {
    std::string result(1, first_char);
    int prox = input.peek();
    while (std::isalnum(prox) || prox == '_') {
        result.append(1, input.get());
        prox = input.peek();
    }
    return result;
}


/**
 * Reads an integer or a real number.
 * Returns the string representation, true if the number is an integer and false
 * if it is a real.
 * TODO: read real number in scientific notation
 * */
std::pair<std::string, bool> read_number(char first_char, Input& input) {
    std::string result(1, first_char);
    bool isinteger = true;
    int prox = input.peek();
    while (std::isdigit(prox) || (prox == '.' && isinteger == true)) {
        char c = input.get();
        result.append(1, c);
        if (c == '.') isinteger = false;
        prox = input.peek();
    }
    return { result, isinteger };
}

/**
 * Read the interior of a string: "<interior>"
 * Returns the string or empty if the expression
 * is not terminated by a "
 * TODO: support escape characters
 * */
std::optional<std::string> read_literal_string(Input& input) {
    std::string result = "";
    char prox = input.get();
    while (prox != '"' && !input.eof()) {
        result.append(1, prox);
        prox = input.get();
    }
    if (input.eof())
        return std::nullopt;
    return result;
}

std::string read_comment(Input& input) {
    while (input.get() != '\n' && !input.eof());
    return "";
}

}
