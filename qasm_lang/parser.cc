#include <cstdlib>
#include <optional>
#include <fstream>
#include "parser.h"

using namespace qasm_lang;


std::string                 parse_header               (std::istream&, unsigned int*);
std::vector<ast::Statement> parse_includes             (std::istream&, unsigned int*);
ast::Statement              parse_statement            (std::istream&, unsigned int*);
ast::IfStatement            parse_if_statement         (std::istream&, unsigned int*);
ast::Expression             parse_expression           (std::istream&, unsigned int*);
ast::Expression             parse_atom_expression      (std::istream&, unsigned int*);
ast::Expression             parse_add_expression       (std::istream&, unsigned int*,
                                                        ast::Expression, bool);
ast::Expression             parse_mult_expression      (std::istream&, unsigned int*,
                                                        ast::Expression, bool);
ast::Variable               parse_argument             (std::istream&, unsigned int*);
ast::ExpressionList         parse_expression_list      (std::istream&, unsigned int*);
ast::IdentifierList         parse_id_list              (std::istream&, unsigned int*);
ast::MixedList              parse_mixed_list           (std::istream&, unsigned int*);
ast::MeasureOperation       parse_measure_operation    (std::istream&, unsigned int*);
ast::ResetOperation         parse_reset_operation      (std::istream&, unsigned int*);
ast::BarrierOperation       parse_barrier_operation    (std::istream&, unsigned int*);
ast::UnitaryOperation       parse_unitary_operation    (std::istream&, unsigned int*,
                                                        ast::UnitaryOperation::Operator,
                                                        std::string);
ast::OpaqueDeclaration      parse_opaque_declaration   (std::istream&, unsigned int*);
ast::GateDeclaration        parse_gate_declaration     (std::istream&, unsigned int*);
ast::VariableDeclaration    parse_variable_declaration (std::istream&, unsigned int*,
                                             ast::VariableDeclaration::DeclarationType);
ast::QuantumOperation       parse_quantum_operation    (std::istream&, unsigned int*);
void                        read_semicolon             (std::istream&, unsigned int*);


ast::Program qasm_lang::parser::parse (std::string filename) {
    using namespace lexer;

    std::fstream input_stream;
    input_stream.open(filename, std::fstream::in);
    if (!input_stream.is_open()) {
        throw ParserError("unable to open file '" + filename + "'", 0, 0);
    }

    unsigned int current_line = 1;
    std::vector<ast::Statement> statements;
    std::vector<ast::Program> includes;

    parse_header(input_stream, &current_line);

    Token tk = next_token(input_stream, &current_line);

    while (tk.type == Token::Type::Keyword_Include) {
        Token include_file = next_token(input_stream, &current_line);
        if (include_file.type != Token::Type::String) {
            throw ParserError("expected a string for the name of the include file",
                              tk.line, tk.column);
        }
        if (include_file.value == filename) {
            throw ParserError("tried to include file from itself: '" + filename + "'",
                              tk.line, tk.column);
        }
        read_semicolon(input_stream, &current_line);
        ast::Program include = parse(include_file.value);
        includes.push_back(include);

        tk = next_token(input_stream, &current_line);
    }

    while (tk.type != Token::Type::Eof) {
        pushback_token(tk);
        statements.push_back(parse_statement(input_stream, &current_line));
        tk = next_token(input_stream, &current_line);
    }

    input_stream.close();
    return ast::Program(filename, statements, includes);
}


/**
 * Parse OpenQasm header and return the requested OpenQasm version.
 * */
std::string parse_header (std::istream& input_stream, unsigned int *current_line) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream, current_line);
    if (tk.type != Token::Keyword_OpenQasm) {
        throw ParserError("expected keyword 'OPENQASM'", tk.line, tk.column);
    }
    Token version_token = next_token(input_stream, current_line);
    if (version_token.type != Token::Real) {
        throw ParserError("expected OpenQasm version number",
                          version_token.line, version_token.column);
    }
    tk = next_token(input_stream, current_line);
    if (tk.type != Token::Symbol_Semicolon) {
        throw ParserError("expected semicolon ';'", tk.line, tk.column);
    }

    return version_token.value;
}


ast::Statement parse_statement (std::istream& input_stream, unsigned int *curr_line) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream, curr_line);
    switch (tk.type) {
    case Token::Keyword_If: {
        auto stmt = parse_if_statement(input_stream, curr_line);
        read_semicolon(input_stream, curr_line);
        return std::move(stmt);
    }
    case Token::Keyword_Opaque: {
        auto stmt = parse_opaque_declaration(input_stream, curr_line);
        read_semicolon(input_stream, curr_line);
        return std::move(stmt);
    }
    case Token::Keyword_Barrier: {
        auto stmt = parse_barrier_operation(input_stream, curr_line);
        read_semicolon(input_stream, curr_line);
        return std::move(stmt);
    }
    case Token::Keyword_Gate: {
        auto stmt = parse_gate_declaration(input_stream, curr_line);
        read_semicolon(input_stream, curr_line);
        return std::move(stmt);
    }
    case Token::Keyword_Creg: {
        auto stmt = parse_variable_declaration(input_stream, curr_line,
                                               ast::VariableDeclaration::Cbit);
        read_semicolon(input_stream, curr_line);
        return std::move(stmt);
    }
    case Token::Keyword_Qreg: {
        auto stmt = parse_variable_declaration(input_stream, curr_line,
                                               ast::VariableDeclaration::Qbit);
        read_semicolon(input_stream, curr_line);
        return std::move(stmt);
    }
    case Token::Keyword_Measure: {
        auto stmt = parse_measure_operation(input_stream, curr_line);
        read_semicolon(input_stream, curr_line);
        return std::move(stmt);
    }
    case Token::Keyword_Reset: {
        auto stmt = parse_reset_operation(input_stream, curr_line);
        read_semicolon(input_stream, curr_line);
        return std::move(stmt);
    }
    case Token::Keyword_U: {
        auto stmt = parse_unitary_operation(input_stream, curr_line,
                                            ast::UnitaryOperation::Operator::U, "U");
        read_semicolon(input_stream, curr_line);
        return std::move(stmt);
    }
    case Token::Keyword_CX: {
        auto stmt = parse_unitary_operation(input_stream, curr_line,
                                            ast::UnitaryOperation::Operator::CX, "CX");
        read_semicolon(input_stream, curr_line);
        return std::move(stmt);
    }
    case Token::Id: {
        auto stmt = parse_unitary_operation(input_stream, curr_line,
                                            ast::UnitaryOperation::Operator::Defined,
                                            tk.value);
        read_semicolon(input_stream, curr_line);
        return std::move(stmt);
    }
    case Token::Keyword_Include: {
        throw ParserError("include directives must be in the beginning of the file",
                          tk.line, tk.column);
    }
    default:
        throw ParserError("unexpected token '" + tk.value + "'", tk.line, tk.column);
    }
}


/**
 * Parse an if statement:
 * if (<id> == <nninteger>) <quantum_op>.
 * The function expecteds the keyworkd `if` to have been read.
 * */
ast::IfStatement parse_if_statement (std::istream& input_stream,
                                     unsigned int *curr_line) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream, curr_line);
    if (tk.type != Token::Type::Symbol_LeftParen) {
        throw ParserError("expected left parenthesis '('", tk.line, tk.column);
    }

    ast::Variable variable = parse_argument(input_stream, curr_line);

    tk = next_token(input_stream, curr_line);
    if (tk.type != Token::Type::Symbol_EqEq) {
        throw ParserError("expected '=='", tk.line, tk.column);
    }

    ast::Expression eq_expression = parse_expression(input_stream, curr_line);

    tk = next_token(input_stream, curr_line);
    if (tk.type != Token::Type::Symbol_RightParen) {
        throw ParserError("expected ')'", tk.line, tk.column);
    }

    ast::QuantumOperation operation = parse_quantum_operation(input_stream, curr_line);
    return ast::IfStatement(variable, eq_expression, operation);
}


/**
 * Parse an opaque declaration:
 * opaque <id> [(<id-list>)] <id-list>
 * */
ast::OpaqueDeclaration parse_opaque_declaration (std::istream& input_stream,
                                                 unsigned int *curr_line) {
    using namespace lexer;
    using namespace parser;

    std::optional<ast::IdentifierList> param_list = std::nullopt;
    Token name = next_token(input_stream, curr_line);

    Token paren = next_token(input_stream, curr_line);
    if (paren.type == Token::Type::Symbol_LeftParen) {
        param_list = parse_id_list(input_stream, curr_line);
        paren = next_token(input_stream, curr_line);
        if (paren.type != Token::Type::Symbol_RightParen) {
            throw ParserError("expected right parenthesis ')'", paren.line, paren.column);
        }
    } else {
        pushback_token(paren);
    }

    ast::IdentifierList arg_list = parse_id_list(input_stream, curr_line);
    return ast::OpaqueDeclaration(name.value, param_list, arg_list, name.line);
}


/**
 * Parse a barrier operation:
 * barrier <any-list>
 * */
ast::BarrierOperation parse_barrier_operation (std::istream& input_stream,
                                               unsigned int *curr_line) {
    using namespace lexer;

    ast::MixedList mixed_list = parse_mixed_list(input_stream, curr_line);
    return ast::BarrierOperation(mixed_list);
}


/**
 * Parse classical or quantum variable declaration.
 * <creg|qreg> <name>\[<dim>\]
 * */
ast::VariableDeclaration parse_variable_declaration (std::istream& input_stream,
                                                     unsigned int *curr_line,
                                         ast::VariableDeclaration::DeclarationType type) {
    using namespace lexer;
    using namespace parser;

    Token name = next_token(input_stream, curr_line);
    if (name.type != Token::Type::Id) {
        throw ParserError("expected an identifier", name.line, name.column);
    }

    Token paren = next_token(input_stream, curr_line);
    if (paren.type != Token::Type::Symbol_LeftSquareParen) {
        throw ParserError("expected a left square parenthesis '['",
                          paren.line, paren.column);
    }

    Token dimension = next_token(input_stream, curr_line);
    if (dimension.type != Token::Type::NonNegativeInteger) {
        throw ParserError("expected the variable dimension",
                          dimension.line, dimension.column);
    }

    paren = next_token(input_stream, curr_line);
    if (paren.type != Token::Type::Symbol_RightSquareParen) {
        throw ParserError("expected a right square parenthesis ']'",
                          name.line, name.column);
    }

    return ast::VariableDeclaration(type, name.value, std::stoul(dimension.value),
                                    name.line);
}


ast::GateDeclaration parse_gate_declaration (std::istream& input_stream,
                                             unsigned int *curr_line) {
    using namespace lexer;
    using namespace parser;

    Token name = next_token(input_stream, curr_line);
    if (name.type != Token::Type::Id) {
        throw ParserError("expected an identifier", name.line, name.column);
    }

    std::optional<ast::IdentifierList> param_list = std::nullopt;
    Token tk = next_token(input_stream, curr_line);
    if (tk.type == Token::Type::Symbol_LeftParen) {
        param_list = parse_id_list(input_stream, curr_line);
        tk = next_token(input_stream, curr_line);
        if (tk.type != Token::Type::Symbol_RightParen) {
            throw ParserError("expected right parenthesis ')'", tk.line, tk.column);
        }
    } else {
        pushback_token(tk);
    }

    ast::IdentifierList arg_list = parse_id_list(input_stream, curr_line);
    tk = next_token(input_stream, curr_line);
    if (tk.type != Token::Type::Symbol_LeftBracket) {
        throw ParserError("expected left bracket '{'", tk.line, tk.column);
    }

    std::vector<ast::GateDeclarationOperation> operations;
    tk = next_token(input_stream, curr_line);
    while (tk.type != Token::Type::Symbol_RightBracket) {
        pushback_token(tk);

        if (tk.type == Token::Type::Keyword_Barrier) {
            operations.push_back(parse_barrier_operation(input_stream, curr_line));
        } else if (tk.type == Token::Type::Keyword_U) {
            operations.push_back(parse_unitary_operation(input_stream, curr_line,
                                                 ast::UnitaryOperation::Operator::U, "U"));
        } else if (tk.type == Token::Type::Keyword_CX) {
            operations.push_back(parse_unitary_operation(input_stream, curr_line,
                                             ast::UnitaryOperation::Operator::CX, "CX"));
        } else if (tk.type == Token::Type::Id) {
            operations.push_back(parse_unitary_operation(input_stream, curr_line,
                                                 ast::UnitaryOperation::Operator::Defined,
                                                         tk.value));
        } else {
            throw ParserError("expected barrier operation or unitary operation",
                              tk.line, tk.column);
        }
        tk = next_token(input_stream, curr_line);
    }

    return ast::GateDeclaration(name.value, param_list, arg_list, operations, name.line);
}


ast::Expression parse_expression (std::istream& input_stream, unsigned int *curr_line) {
    using namespace lexer;

    ast::Expression left = parse_atom_expression(input_stream, curr_line);
    Token tk = next_token(input_stream, curr_line);
    if (tk.type == Token::Type::Symbol_Plus) {
        return parse_add_expression(input_stream, curr_line, left, false);
    } else if (tk.type == Token::Type::Symbol_Minus) {
        return parse_add_expression(input_stream, curr_line, left, true);
    } else if (tk.type == Token::Type::Symbol_Times) {
        return parse_mult_expression(input_stream, curr_line, left, false);
    } else if (tk.type == Token::Type::Symbol_Divide) {
        return parse_mult_expression(input_stream, curr_line, left, true);
    } else {
        return left;
    }
}

/**
 * Parse atom expression, where atom expression is:
 *  - a literal number or contanst
 *  - a unary call (sin, cos, ...)
 *  - an expression in parenthesis
 * */
ast::Expression parse_atom_expression (std::istream& input_stream,
                                       unsigned int *curr_line) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream, curr_line);
    switch (tk.type) {
    case Token::Type::Real:
        return ast::RealNumber(tk.value);

    case Token::Type::NonNegativeInteger:
        return ast::NonNegativeInteger(tk.value);

    case Token::Type::Keyword_Pi:
        return ast::EspecialConstant(ast::EspecialConstant::Pi);

    case Token::Type::Id:
        return ast::Variable(tk.value);

    case Token::Type::Keyword_Cos:
    case Token::Type::Keyword_Sin:
    case Token::Type::Keyword_Tan:
    case Token::Type::Keyword_Exp:
    case Token::Type::Keyword_Ln:
    case Token::Type::Keyword_Sqrt: {
        Token left_paren = next_token(input_stream, curr_line);
        if (left_paren.type == Token::Symbol_LeftParen) {
            ast::Expression expr = parse_expression(input_stream, curr_line);
            Token right_paren = next_token(input_stream, curr_line);
            if (right_paren.type == Token::Type::Symbol_RightParen) {
                if (tk.type == Token::Type::Keyword_Cos)
                    return ast::UnaryOperation(ast::UnaryOperation::Cos, expr);
                else if (tk.type == Token::Type::Keyword_Sin)
                    return ast::UnaryOperation(ast::UnaryOperation::Sin, expr);
                else if (tk.type == Token::Type::Keyword_Tan)
                    return ast::UnaryOperation(ast::UnaryOperation::Tan, expr);
                else if (tk.type == Token::Type::Keyword_Ln)
                    return ast::UnaryOperation(ast::UnaryOperation::Ln, expr);
                else if (tk.type == Token::Type::Keyword_Exp)
                    return ast::UnaryOperation(ast::UnaryOperation::Exp, expr);
                else
                    return ast::UnaryOperation(ast::UnaryOperation::Sqrt, expr);
            } else {
                throw ParserError("expected right parenthesis ')'",
                                  right_paren.line, right_paren.column);
            }
        } else {
            throw ParserError("expected left parenthesis '(' after unary operator",
                              left_paren.line, left_paren.column);
        }
    }

    case Token::Type::Symbol_Minus: {
        ast:: Expression expr = parse_expression(input_stream, curr_line);
        return ast::MinusExpression(expr);
    }

    case Token::Type::Symbol_LeftParen: {
        ast::Expression expr = parse_expression(input_stream, curr_line);
        Token right_paren = next_token(input_stream, curr_line);
        if (right_paren.type != Token::Symbol_RightParen) {
            throw ParserError("expected right parenthesis ')'",
                              right_paren.line, right_paren.column);
        }
        return expr;
    }

    default:
        throw ParserError("unexpected token '" + tk.value + "'", tk.line, tk.column);
    }
}

/**
 * Parse a binary additive operation: a sum or difference.
 * The function expects the binary operation symbol to have been read. The boolean
 * argument `minus` should true if a `-` was read, and false if `+` was read.
 * */
ast::Expression parse_add_expression (std::istream& input_stream, unsigned int *curr_line,
                                      ast::Expression left, bool minus) {
    using namespace lexer;

    auto _left = left;
    bool _minus = minus;
    while (true) {
        ast::Expression atom_expression = parse_atom_expression(input_stream, curr_line);
        Token tk = next_token(input_stream, curr_line);

        ast::Expression right;
        if (tk.type == Token::Type::Symbol_Times) {
            right = parse_mult_expression(input_stream, curr_line, atom_expression, false);
            tk = next_token(input_stream, curr_line);
        } else if (tk.type == Token::Type::Symbol_Divide) {
            right = parse_mult_expression(input_stream, curr_line, atom_expression, true);
            tk = next_token(input_stream, curr_line);
        } else {
            right = atom_expression;
        }

        if (_minus) {
            auto __left = _left;
            _left = ast::SubtractionExpression(__left, right);
        } else {
            auto __left = _left;
            _left = ast::AdditionExpression(__left, right);
        }

        if (tk.type == Token::Type::Symbol_Plus) {
            _minus = false;
        } else if (tk.type == Token::Type::Symbol_Minus) {
            _minus = true;
        } else {
            pushback_token(tk);
            return _left;
        }
    }
}


/**
 * Parse a binary multiplicative operation: a multiplication or a division.
 * The function expects the binary operation symbol to have been read. The boolean
 * argument `division` should true if a `/` was read, and false if `*` was read.
 * */
ast::Expression parse_mult_expression (std::istream& input_stream, unsigned int *curr_line,
                                       ast::Expression left, bool division) {
    using namespace lexer;

    auto _left = left;
    auto _division = division;
    while (true) {
        ast::Expression right = parse_atom_expression(input_stream, curr_line);
        if (_division) {
            auto __left = _left;
            _left = ast::DivisionExpression(__left, right);
        } else {
            auto __left = _left;
            _left = ast::MultiplicationExpression(__left, right);
        }

        Token tk = lexer::next_token(input_stream, curr_line);
        if (tk.type == Token::Type::Symbol_Divide) {
            _division = true;
        } else if (tk.type == Token::Type::Symbol_Times) {
            _division = false;
        } else {
            lexer::pushback_token(tk);
            return _left;
        }
    }
}


/**
 * Parse an expression list:
 * <expression> | <expression> , <expression-list>
 * */
ast::ExpressionList parse_expression_list (std::istream& input_stream,
                                           unsigned int* curr_line) {
    using namespace lexer;

    ast::Expression first = parse_expression(input_stream, curr_line);
    std::vector<ast::Expression> expression_list;
    expression_list.push_back(first);

    while (true) {
        Token comma = next_token(input_stream, curr_line);
        if (comma.type != Token::Type::Symbol_Comma) {
            break;
        }
        ast::Expression expr = parse_expression(input_stream, curr_line);
        expression_list.push_back(expr);
    }

    return ast::ExpressionList(expression_list);
}


/**
 * Parse a list of identifiers:
 * <id> | <id>, <id-list>
 * */
ast::IdentifierList parse_id_list (std::istream& input_stream, unsigned int* curr_line) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream, curr_line);
    if (tk.type != Token::Type::Id) {
        throw ParserError("expected an identifier", tk.line, tk.column);
    }

    std::vector<std::string> id_list;
    id_list.push_back(tk.value);

    while (true) {
        Token comma = next_token(input_stream, curr_line);
        if (comma.type != Token::Type::Symbol_Comma) {
            break;
        }
        tk = next_token(input_stream, curr_line);
        if (tk.type != Token::Type::Id) {
            throw ParserError("expected an identifier", tk.line, tk.column);
        }
        id_list.push_back(tk.value);
    }

    return ast::IdentifierList(id_list);
}


/**
 * Parse a list that can contain variables and variable indexing's
 * (<id> | <id>[<index>]) | (<id> | <id>[<index>]), <mixed-list>
 * */
ast::MixedList parse_mixed_list (std::istream& input_stream, unsigned int* curr_line) {
    using namespace lexer;

    ast::Variable first = parse_argument(input_stream, curr_line);
    std::vector<ast::Variable> mixed_list;
    mixed_list.push_back(first);

    while (true) {
        Token comma = next_token(input_stream, curr_line);
        if (comma.type != Token::Type::Symbol_Comma) {
            break;
        }

        ast::Variable var = parse_argument(input_stream, curr_line);
        mixed_list.push_back(var);
    }

    return ast::MixedList(mixed_list);
}


/**
 * An argument is either <identifier> or <identifier>[<index>]
 * */
ast::Variable parse_argument (std::istream& input_stream, unsigned int* curr_line) {
    using namespace lexer;
    using namespace parser;

    Token identifier = next_token(input_stream, curr_line);
    if (identifier.type != Token::Type::Id) {
        throw ParserError("expected an identifier", identifier.line, identifier.column);
    }

    Token tk = next_token(input_stream, curr_line);
    if (tk.type == Token::Type::Symbol_LeftSquareParen) {
        Token index = next_token(input_stream, curr_line);
        if (index.type != Token::Type::NonNegativeInteger) {
            throw ParserError("expected vector index", index.line, index.column);
        }
        tk = next_token(input_stream, curr_line);
        if (tk.type != Token::Type::Symbol_RightSquareParen) {
            throw ParserError("expected right square bracket ']'",
                              index.line, index.column);
        }
        return ast::Variable(identifier.value, std::stoul(index.value));
    } else {
        pushback_token(tk);
        return ast::Variable(identifier.value);
    }
}


/**
 * Parse measure operation:
 * measure <arg> -> <arg>;
 * The function expects the measure keyword to have been read.
 * */
ast::MeasureOperation parse_measure_operation (std::istream& input_stream,
                                               unsigned int* curr_line) {
    using namespace lexer;
    using namespace parser;

    ast::Variable source_arg = parse_argument(input_stream, curr_line);
    Token tk = next_token(input_stream, curr_line);
    if (tk.type != Token::Type::Symbol_RightArrow) {
        throw ParserError("expected right arrow '->'", tk.line, tk.column);
    }
    ast::Variable target_arg = parse_argument(input_stream, curr_line);
    return ast::MeasureOperation(source_arg, target_arg);
}


/**
 * Parse a reset operation:
 * reset <arg>;
 * The function expects the reset keyword to have been read.
 * */
ast::ResetOperation parse_reset_operation (std::istream& input_stream,
                                           unsigned int* curr_line) {
    using namespace lexer;
    using namespace parser;

    ast::Variable target = parse_argument(input_stream, curr_line);
    return ast::ResetOperation(target);
}

/**
 * Parse a unitary operator application:
 * U(<expression_list>) <arg>;
 * CX <arg> <arg>;
 * <operator> <anylist>;
 * <operator> () <anylist>;
 * <operator> (<expression_list>) <anylist>;
 * */
ast::UnitaryOperation parse_unitary_operation (std::istream& input_stream,
                                                    unsigned int* curr_line,
                                                    ast::UnitaryOperation::Operator op,
                                                    std::string operator_name) {
    using namespace lexer;
    using namespace parser;

    if (op == ast::UnitaryOperation::Operator::U) {
        Token paren = next_token(input_stream, curr_line);
        if (paren.type != Token::Type::Symbol_LeftParen) {
            throw ParserError("expected left parenthesis '('", paren.line, paren.column);
        }

        ast::ExpressionList exp_list = parse_expression_list(input_stream, curr_line);
        ast::Variable arg = parse_argument(input_stream, curr_line);
        ast::MixedList arg_list = ast::MixedList({ arg });

        paren = next_token(input_stream, curr_line);
        if (paren.type != Token::Type::Symbol_RightParen) {
            throw ParserError("expected right parenthesis ')'", paren.line, paren.column);
        }

        return ast::UnitaryOperation(op, operator_name, exp_list, arg_list);
    } else if (op == ast::UnitaryOperation::Operator::CX) {
        ast::Variable arga = parse_argument(input_stream, curr_line);
        ast::Variable argb = parse_argument(input_stream, curr_line);
        ast::MixedList arg_list = ast::MixedList({ arga, argb });
        return ast::UnitaryOperation(op, operator_name, std::nullopt, arg_list);
    } else {
        Token paren = next_token(input_stream, curr_line);
        if (paren.type == Token::Type::Symbol_LeftParen) {
            std::optional<ast::ExpressionList> exp_list = std::nullopt;
            paren = next_token(input_stream, curr_line);
            if (paren.type != Token::Type::Symbol_RightParen) {
                pushback_token(paren);
                exp_list = parse_expression_list(input_stream, curr_line);
                paren = next_token(input_stream, curr_line);
                if (paren.type != Token::Type::Symbol_RightParen) {
                    throw ParserError("expected right parenthesis ')'", paren.line,
                                                                        paren.column);
                }
            }
            ast::MixedList arg_list = parse_mixed_list(input_stream, curr_line);
            return ast::UnitaryOperation(op, operator_name, exp_list, arg_list);
        } else {
            pushback_token(paren);
            ast::MixedList arg_list = parse_mixed_list(input_stream, curr_line);
            return ast::UnitaryOperation(op, operator_name, std::nullopt, arg_list);
        }
    }
}


ast::QuantumOperation parse_quantum_operation (std::istream& input_stream,
                                               unsigned int* curr_line) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream, curr_line);
    if (tk.type == Token::Type::Keyword_Measure) {
        return parse_measure_operation(input_stream, curr_line);
    } else if (tk.type == Token::Type::Keyword_Reset) {
        return parse_reset_operation(input_stream, curr_line);
    } else if (tk.type == Token::Type::Keyword_U) {
        return parse_unitary_operation(input_stream, curr_line,
                                       ast::UnitaryOperation::Operator::U, "U");
    } else if (tk.type == Token::Type::Keyword_CX) {
        return parse_unitary_operation(input_stream, curr_line,
                                       ast::UnitaryOperation::Operator::CX, "CX");
    } else if (tk.type == Token::Type::Id) {
        return parse_unitary_operation(input_stream, curr_line,
                                       ast::UnitaryOperation::Operator::Defined,
                                       tk.value);
    } else {
        throw ParserError("expected a quantum operation: reset, measure or unitary",
                          tk.line, tk.column);
    }
}

void read_semicolon (std::istream& input_stream, unsigned int* curr_line) {
    using namespace lexer;
    using namespace parser;
    Token tk = next_token(input_stream, curr_line);
    if (tk.type != Token::Symbol_Semicolon) {
        throw ParserError("expected semicolon ';'", tk.line, tk.column);
    }
}
