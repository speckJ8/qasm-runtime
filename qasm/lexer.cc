#include "lexer.h"

#include <cctype>
#include <utility>
#include <sstream>
#include <optional>
#include <stack>
#include <iostream>

std::stack<qasm::lexer::Token> _token_stack;

const std::string qasm::lexer::Token::OpenQasm = "OPENQASM";
const std::string qasm::lexer::Token::Include  = "include";
const std::string qasm::lexer::Token::Opaque   = "opaque";
const std::string qasm::lexer::Token::If       = "if";
const std::string qasm::lexer::Token::Barrier  = "barrier";
const std::string qasm::lexer::Token::Qreg     = "qreg";
const std::string qasm::lexer::Token::Creg     = "creg";
const std::string qasm::lexer::Token::Gate     = "gate";
const std::string qasm::lexer::Token::Measure  = "measure";
const std::string qasm::lexer::Token::Reset    = "reset";
const std::string qasm::lexer::Token::U        = "U";
const std::string qasm::lexer::Token::CX       = "CX";
const std::string qasm::lexer::Token::Pi       = "pi";
const std::string qasm::lexer::Token::Cos      = "cos";
const std::string qasm::lexer::Token::Sin      = "sin";
const std::string qasm::lexer::Token::Tan      = "tan";
const std::string qasm::lexer::Token::Exp      = "exp";
const std::string qasm::lexer::Token::Ln       = "ln";
const std::string qasm::lexer::Token::Sqrt     = "sqrt";


std::pair<unsigned int, unsigned int> strip_spaces(std::istream& stream);
std::pair<std::string, bool> read_number(char first_char, std::istream& stream);
std::string read_string(char first_char, std::istream& stream);
std::optional<std::string> read_literal_string(std::istream& stream);
std::string read_comment(std::istream& stream);


std::string qasm::lexer::repr(qasm::lexer::Token::Type type) {
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

qasm::lexer::Token qasm::lexer::_next_token(std::istream& stream) {
    if (!_token_stack.empty()) {
        auto tk = std::move(_token_stack.top());
        _token_stack.pop();
        return tk;
    }

    auto [ line, column ] = strip_spaces(stream);
    _line += line;
    if (line > 0) {
        qasm::lexer::_column = column;
    } else {
        qasm::lexer::_column += column;
    }
    char next = stream.get();
    auto col = qasm::lexer::_column;

    if (stream.eof())
        return Token(Token::Eof, Token::EofK, "", _line, col);
 
    qasm::lexer::_column++;

    switch (next) {
    case '{':
        return Token(Token::Symbol_LeftBracket, Token::Symbol, "{", _line, col);
    case '}':
        return Token(Token::Symbol_RightBracket, Token::Symbol, "}", _line, col);
    case ';':
        return Token(Token::Symbol_Semicolon, Token::Symbol, ";", _line, col);
    case ',':
        return Token(Token::Symbol_Comma, Token::Symbol, ",", _line, col);
    case '(':
        return Token(Token::Symbol_LeftParen, Token::Symbol, "(", _line, col);
    case ')':
        return Token(Token::Symbol_RightParen, Token::Symbol, ")", _line, col);
    case '[':
        return Token(Token::Symbol_LeftSquareParen, Token::Symbol, "[", _line, col);
    case ']':
        return Token(Token::Symbol_RightSquareParen, Token::Symbol, "]", _line, col);
    case '+':
        return Token(Token::Symbol_Plus, Token::Symbol, "+", _line, col);
    case '-':
        if (stream.peek() == '>') {
            stream.get();
            return Token(Token::Symbol_RightArrow, Token::Symbol, "->", _line, col);
        }
        return Token(Token::Symbol_Minus, Token::Symbol, "-", _line, col);
    case '*':
        return Token(Token::Symbol_Times, Token::Symbol, "*", _line, col);
    case '/':
        return Token(Token::Symbol_Divide, Token::Symbol, "/", _line, col);
    case '^':
        return Token(Token::Symbol_Exp, Token::Symbol, "^", _line, col);
    case '=':
        if (stream.get() == '=') {
            qasm::lexer::_column++;
            return Token(Token::Symbol_EqEq, Token::Symbol, "==", _line, col);
        } else {
            return Token(Token::Invalid, Token::InvalidK, "", _line, col);
        }
    case '#':
        return Token(Token::Comment, Token::CommentK, read_comment(stream), _line, col);
    }

    if (std::isalpha(next) || next == '_') {
        std::string text = read_string(next, stream);
        qasm::lexer::_column += text.size() - 1;
        if (text == Token::OpenQasm)
            return Token(Token::Keyword_OpenQasm, Token::Keyword, text, _line, col);
        if (text == Token::Include)
            return Token(Token::Keyword_Include, Token::Keyword, text, _line, col);
        if (text == Token::Opaque)
            return Token(Token::Keyword_Opaque, Token::Keyword, text, _line, col);
        if (text == Token::If)
            return Token(Token::Keyword_If, Token::Keyword, text, _line, col);    
        if (text == Token::Barrier)
            return Token(Token::Keyword_Barrier, Token::Keyword, text, _line, col);
        if (text == Token::Qreg)
            return Token(Token::Keyword_Qreg, Token::Keyword, text, _line, col);
        if (text == Token::Creg)
            return Token(Token::Keyword_Creg, Token::Keyword, text, _line, col);
        if (text == Token::Gate)
            return Token(Token::Keyword_Gate, Token::Keyword, text, _line, col);
        if (text == Token::Measure)
            return Token(Token::Keyword_Measure, Token::Keyword, text, _line, col);
        if (text == Token::Reset)
            return Token(Token::Keyword_Reset, Token::Keyword, text, _line, col);
        if (text == Token::U)
            return Token(Token::Keyword_U, Token::Keyword, text, _line, col);
        if (text == Token::CX)
            return Token(Token::Keyword_CX, Token::Keyword, text, _line, col);
        if (text == Token::Pi)
            return Token(Token::Keyword_Pi, Token::Keyword, text, _line, col);
        if (text == Token::Cos)
            return Token(Token::Keyword_Cos, Token::Keyword, text, _line, col);
        if (text == Token::Sin)
            return Token(Token::Keyword_Cos, Token::Keyword, text, _line, col);
        if (text == Token::Tan)
            return Token(Token::Keyword_Tan, Token::Keyword, text, _line, col);
        if (text == Token::Exp)
            return Token(Token::Keyword_Exp, Token::Keyword, text, _line, col);
        if (text == Token::Ln)
            return Token(Token::Keyword_Ln, Token::Keyword, text, _line, col);
        if (text == Token::Sqrt)
            return Token(Token::Keyword_Sqrt, Token::Keyword, text, _line, col);

        return Token(Token::Id, Token::Identifier, text, _line, col);
    }
    else if (std::isdigit(next)) {
        auto [ num, isinteger ] = read_number(next, stream);
        qasm::lexer::_column += num.size() - 1;
        if (isinteger)
            return Token(Token::NonNegativeInteger, Token::Literal, num, _line, col);
        else
            return Token(Token::Real, Token::Literal, num, _line, col);
    }
    else if (next == '"') {
        auto str = read_literal_string(stream);
        if (str) {
            col = qasm::lexer::_column;
            qasm::lexer::_column += str.value().size() + 1;
            return Token(Token::String, Token::Literal, str.value(), _line, col);
        }
        else
            return Token(Token::Invalid, Token::InvalidK, "", _line, col);
    }
    else {
        return Token(Token::Invalid, Token::InvalidK, std::string(1, next), _line, col);
    }
}
qasm::lexer::Token qasm::lexer::next_token(std::istream& input_stream) {
    auto tok = _next_token(input_stream);
    // std::cout << "produced " << tok.value << "\n";
    return tok;
}

void qasm::lexer::pushback_token(qasm::lexer::Token tk) {
    // std::cout << "pushback " << tk.value << "\n";
    _token_stack.push(tk);
}

/**
 * Skip blanks from `stream` ('\n', '\t', '\f', '\r', '\v' and ' ').
 * Returns the number of line breaks encounterd and the number of spaces blanks
 * the last line break.
 * */
std::pair<unsigned int, unsigned int> strip_spaces(std::istream& stream) {
    unsigned int nr_breaks = 0;
    unsigned int last_spaces = 0;

    while (std::isspace(stream.peek()) && !stream.eof()) {
        char next = stream.get();
        if (next == '\n' || next == '\r') {
            nr_breaks++;
            last_spaces = 0;
        } else {
            last_spaces ++;
        }
    }

    return { nr_breaks, last_spaces };
}

std::string read_string(char first_char, std::istream& stream) {
    std::string result(1, first_char);
    int prox = stream.peek();
    while (std::isalnum(prox) || prox == '_') {
        result.append(1, stream.get());
        prox = stream.peek();
    }
    return result;
}


/**
 * Reads an integer or a real number.
 * Returns the string representation, true if the number is an integer and false
 * if it is a real.
 * TODO: read real number in scientific notation
 * */
std::pair<std::string, bool> read_number(char first_char, std::istream& stream) {
    std::string result(1, first_char);
    bool isinteger = true;
    int prox = stream.peek();
    while (std::isdigit(prox) || (prox == '.' && isinteger == true)) {
        char c = stream.get();
        result.append(1, c);
        if (c == '.') isinteger = false;
        prox = stream.peek();
    }
    return { result, isinteger };
}

/**
 * Read the interior of a string: "<interior>"
 * Returns the string or empty if the expression
 * is not terminated by a "
 * TODO: support escape characters
 * */
std::optional<std::string> read_literal_string(std::istream& stream) {
    std::string result = "";
    char prox = stream.get();
    while (prox != '"' && !stream.eof()) {
        result.append(1, prox);
        prox = stream.get();
    }
    if (stream.eof())
        return std::nullopt;
    return result;
}

std::string read_comment(std::istream& stream) {
    while (stream.get() != '\n' && !stream.eof());
    return "";
}
