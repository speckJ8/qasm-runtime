#include <cstdlib>
#include <optional>
#include <fstream>
#include "parser.h"


namespace qasm {

std::string                      parse_header               (std::istream&, std::string);
std::shared_ptr<ast::Statement>  parse_statement            (std::istream&);
ast::Variable                    parse_argument             (std::istream&);
ast::ExpressionList              parse_expression_list      (std::istream&);
ast::IdentifierList              parse_id_list              (std::istream&);
ast::MixedList                   parse_mixed_list           (std::istream&);
ast::IfStatement                 parse_if_statement         (std::istream&, ast::Context);
ast::MeasureOperation            parse_measure_operation    (std::istream&, ast::Context);
ast::ResetOperation              parse_reset_operation      (std::istream&, ast::Context);
ast::BarrierOperation            parse_barrier_operation    (std::istream&, ast::Context);
ast::UnitaryOperation            parse_unitary_operation    (std::istream&, ast::Context,
                                                             ast::UnitaryOperation::Operator,
                                                             std::string);
ast::OpaqueDeclaration           parse_opaque_declaration   (std::istream&, ast::Context);
ast::GateDeclaration             parse_gate_declaration     (std::istream&, ast::Context);
ast::VariableDeclaration         parse_variable_declaration (
                                                 std::istream&, ast::Context,
                                                 ast::VariableDeclaration::DeclarationType);
std::shared_ptr<ast::Statement>  parse_quantum_operation    (std::istream&);
std::shared_ptr<ast::Expression> parse_expression           (std::istream&);
std::shared_ptr<ast::Expression> parse_atom_expression      (std::istream&);
std::shared_ptr<ast::Expression> parse_add_expression       (std::istream&,
                                                         std::shared_ptr<ast::Expression>,
                                                             bool);
std::shared_ptr<ast::Expression> parse_mult_expression      (std::istream&,
                                                         std::shared_ptr<ast::Expression>,
                                                             bool);
void                             read_semicolon             (std::istream&);


ast::Program parser::parse(std::string filename) {
    using namespace lexer;

    std::fstream input_stream;
    input_stream.open(filename, std::fstream::in);
    if (!input_stream.is_open()) {
        throw ParserError("unable to open file '" + filename + "'", 0, 0);
    }

    std::vector<std::shared_ptr<ast::Statement>> statements;
    std::vector<ast::Program> includes;

    parse_header(input_stream, filename);

    Token tk = next_token(input_stream);

    while (tk.type == Token::Type::Keyword_Include) {
        Token include_file = next_token(input_stream);
        if (include_file.type != Token::Type::String) {
            throw ParserError("expected a string for the name of the include file",
                              tk.line, tk.column);
        }
        if (include_file.value == filename) {
            throw ParserError("tried to include file from itself: '" + filename + "'",
                              tk.line, tk.column);
        }
        read_semicolon(input_stream);
        ast::Program include = parse(include_file.value);
        includes.push_back(std::move(include));

        tk = next_token(input_stream);
    }

    while (tk.type != Token::Type::Eof) {
        pushback_token(tk);
        try {
            std::shared_ptr<ast::Statement> stmt = parse_statement(input_stream);
            statements.push_back(std::move(stmt));
        } catch (const ParserError& err) {
            throw ParserError(err.msg, err.line, err.column, filename);
        }
        tk = next_token(input_stream);
    }

    input_stream.close();
    return ast::Program(filename, std::move(statements), std::move(includes));
}


/**
 * Parse OpenQasm header and return the requested OpenQasm version.
 * */
std::string parse_header(std::istream& input_stream, std::string filename) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream);
    if (tk.type != Token::Keyword_OpenQasm) {
        throw ParserError("expected keyword 'OPENQASM'", tk.line, tk.column, filename);
    }
    Token version_token = next_token(input_stream);
    if (version_token.type != Token::Real) {
        throw ParserError("expected OpenQasm version number",
                          version_token.line, version_token.column, filename);
    }
    tk = next_token(input_stream);
    if (tk.type != Token::Symbol_Semicolon) {
        throw ParserError("expected semicolon ';'", tk.line, tk.column, filename);
    }

    return version_token.value;
}


std::shared_ptr<ast::Statement> parse_statement(std::istream& input_stream) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream);
    auto context = ast::Context(tk.line, tk.column);

    switch (tk.type) {
    case Token::Keyword_If: {
        auto stmt = parse_if_statement(input_stream, context);
        read_semicolon(input_stream);
        return std::make_shared<ast::IfStatement>(std::move(stmt));
    }
    case Token::Keyword_Opaque: {
        auto stmt = parse_opaque_declaration(input_stream, context);
        read_semicolon(input_stream);
        return std::make_shared<ast::OpaqueDeclaration>(std::move(stmt));
    }
    case Token::Keyword_Barrier: {
        auto stmt = parse_barrier_operation(input_stream, context);
        read_semicolon(input_stream);
        return std::make_shared<ast::BarrierOperation>(std::move(stmt));
    }
    case Token::Keyword_Gate: {
        auto stmt = parse_gate_declaration(input_stream, context);
        //read_semicolon(input_stream);
        return std::make_shared<ast::GateDeclaration>(std::move(stmt));
    }
    case Token::Keyword_Creg: {
        auto stmt = parse_variable_declaration(input_stream, context,
                                               ast::VariableDeclaration::Cbit);
        read_semicolon(input_stream);
        return std::make_shared<ast::VariableDeclaration>(std::move(stmt));
    }
    case Token::Keyword_Qreg: {
        auto stmt = parse_variable_declaration(input_stream, context,
                                               ast::VariableDeclaration::Qbit);
        read_semicolon(input_stream);
        return std::make_shared<ast::VariableDeclaration>(std::move(stmt));
    }
    case Token::Keyword_Measure: {
        auto stmt = parse_measure_operation(input_stream, context);
        read_semicolon(input_stream);
        return std::make_shared<ast::MeasureOperation>(std::move(stmt));
    }
    case Token::Keyword_Reset: {
        auto stmt = parse_reset_operation(input_stream, context);
        read_semicolon(input_stream);
        return std::make_shared<ast::ResetOperation>(std::move(stmt));
    }
    case Token::Keyword_U: {
        auto stmt = parse_unitary_operation(input_stream, context,
                                            ast::UnitaryOperation::Operator::U, "U");
        read_semicolon(input_stream);
        return std::make_shared<ast::UnitaryOperation>(std::move(stmt));
    }
    case Token::Keyword_CX: {
        auto stmt = parse_unitary_operation(input_stream, context,
                                            ast::UnitaryOperation::Operator::CX, "CX");
        read_semicolon(input_stream);
        return std::make_shared<ast::UnitaryOperation>(std::move(stmt));
    }
    case Token::Id: {
        auto stmt = parse_unitary_operation(input_stream, context,
                                            ast::UnitaryOperation::Operator::Defined,
                                            tk.value);
        read_semicolon(input_stream);
        return std::make_shared<ast::UnitaryOperation>(std::move(stmt));
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
ast::IfStatement parse_if_statement(std::istream& input_stream, ast::Context c) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream);
    if (tk.type != Token::Type::Symbol_LeftParen) {
        throw ParserError("expected left parenthesis '('", tk.line, tk.column);
    }

    ast::Variable variable = parse_argument(input_stream);

    tk = next_token(input_stream);
    if (tk.type != Token::Type::Symbol_EqEq) {
        throw ParserError("expected '=='", tk.line, tk.column);
    }

    auto eq_expression = parse_expression(input_stream);

    tk = next_token(input_stream);
    if (tk.type != Token::Type::Symbol_RightParen) {
        throw ParserError("expected ')'", tk.line, tk.column);
    }

    std::shared_ptr<ast::Statement> operation = parse_quantum_operation(input_stream);
    return ast::IfStatement(variable, std::move(eq_expression), std::move(operation), c);
}


/**
 * Parse an opaque declaration:
 * opaque <id> [(<id-list>)] <id-list>
 * */
ast::OpaqueDeclaration parse_opaque_declaration(std::istream& input_stream,ast::Context c) {
    using namespace lexer;
    using namespace parser;

    std::optional<ast::IdentifierList> param_list = std::nullopt;
    Token name = next_token(input_stream);

    Token paren = next_token(input_stream);
    if (paren.type == Token::Type::Symbol_LeftParen) {
        param_list = parse_id_list(input_stream);
        paren = next_token(input_stream);
        if (paren.type != Token::Type::Symbol_RightParen) {
            throw ParserError("expected right parenthesis ')'", paren.line, paren.column);
        }
    } else {
        pushback_token(paren);
    }

    ast::IdentifierList arg_list = parse_id_list(input_stream);
    return ast::OpaqueDeclaration(name.value, param_list, arg_list, c);
}


/**
 * Parse a barrier operation:
 * barrier <any-list>
 * */
ast::BarrierOperation parse_barrier_operation(std::istream& input_stream, ast::Context c) {
    using namespace lexer;

    ast::MixedList mixed_list = parse_mixed_list(input_stream);
    return ast::BarrierOperation(mixed_list, c);
}


/**
 * Parse classical or quantum variable declaration.
 * <creg|qreg> <name>\[<dim>\]
 * */
ast::VariableDeclaration parse_variable_declaration(
            std::istream& input_stream,
            ast::Context c,
            ast::VariableDeclaration::DeclarationType type)
{
    using namespace lexer;
    using namespace parser;

    Token name = next_token(input_stream);
    if (name.type != Token::Type::Id) {
        throw ParserError("expected an identifier, but found " + name.value,
                          name.line, name.column);
    }

    Token paren = next_token(input_stream);
    if (paren.type != Token::Type::Symbol_LeftSquareParen) {
        throw ParserError("expected a left square parenthesis '[', but found " + paren.value,
                          paren.line, paren.column);
    }

    Token dimension = next_token(input_stream);
    if (dimension.type != Token::Type::NonNegativeInteger) {
        throw ParserError("expected the variable dimension, but found " + dimension.value,
                          dimension.line, dimension.column);
    }

    paren = next_token(input_stream);
    if (paren.type != Token::Type::Symbol_RightSquareParen) {
        throw ParserError("expected a right square parenthesis ']', but found " +
                              paren.value,
                          paren.line, paren.column);
    }

    return ast::VariableDeclaration(type, name.value, std::stoul(dimension.value), c);
}


ast::GateDeclaration parse_gate_declaration(std::istream& input_stream, ast::Context c) {
    using namespace lexer;
    using namespace parser;

    Token name = next_token(input_stream);
    if (name.type != Token::Type::Id) {
        throw ParserError("expected an identifier, but found " + name.value,
                          name.line, name.column);
    }

    std::optional<ast::IdentifierList> param_list = std::nullopt;
    Token tk = next_token(input_stream);
    if (tk.type == Token::Type::Symbol_LeftParen) {
        param_list = parse_id_list(input_stream);
        tk = next_token(input_stream);
        if (tk.type != Token::Type::Symbol_RightParen) {
            throw ParserError("expected right parenthesis ')', but found " + tk.value,
                              tk.line, tk.column);
        }
    } else {
        pushback_token(tk);
    }

    ast::IdentifierList arg_list = parse_id_list(input_stream);

    tk = next_token(input_stream);
    if (tk.type != Token::Type::Symbol_LeftBracket) {
        throw ParserError("expected left bracket '{', but found " + tk.value,
                          tk.line, tk.column);
    }

    std::vector<std::shared_ptr<ast::Statement>> operations;

    tk = next_token(input_stream);
    auto ctx = ast::Context(tk.line, tk.column);

    while (tk.type != Token::Type::Symbol_RightBracket) {
        if (tk.type == Token::Type::Keyword_Barrier) {
            auto stmt = parse_barrier_operation(input_stream, ctx);
            operations.push_back(std::make_shared<ast::BarrierOperation>(std::move(stmt)));
        } else if (tk.type == Token::Type::Keyword_U) {
            auto stmt = parse_unitary_operation(input_stream, ctx,
                                                ast::UnitaryOperation::Operator::U, "U");
            operations.push_back(std::make_shared<ast::UnitaryOperation>(std::move(stmt)));
        } else if (tk.type == Token::Type::Keyword_CX) {
            auto stmt = parse_unitary_operation(input_stream, ctx,
                                                ast::UnitaryOperation::Operator::CX, "CX");
            operations.push_back(std::make_shared<ast::UnitaryOperation>(std::move(stmt)));
        } else if (tk.type == Token::Type::Id) {
            auto stmt = parse_unitary_operation(input_stream, ctx,
                                                ast::UnitaryOperation::Operator::Defined,
                                                tk.value);
            operations.push_back(std::make_shared<ast::UnitaryOperation>(std::move(stmt)));
        } else {
            throw ParserError("expected barrier operation or unitary operation, but found "+
                                  tk.value,
                              tk.line, tk.column);
        }
        read_semicolon(input_stream);
        tk = next_token(input_stream);
    }

    return ast::GateDeclaration(name.value, std::move(param_list), arg_list,
                                std::move(operations), c);
}


std::shared_ptr<ast::Expression> parse_expression(std::istream& input_stream) {
    using namespace lexer;

    std::shared_ptr<ast::Expression> left = parse_atom_expression(input_stream);
    Token tk = next_token(input_stream);
    if (tk.type == Token::Type::Symbol_Plus) {
        return parse_add_expression(input_stream, std::move(left), false);
    } else if (tk.type == Token::Type::Symbol_Minus) {
        return parse_add_expression(input_stream, std::move(left), true);
    } else if (tk.type == Token::Type::Symbol_Times) {
        return parse_mult_expression(input_stream, std::move(left), false);
    } else if (tk.type == Token::Type::Symbol_Divide) {
        return parse_mult_expression(input_stream, std::move(left), true);
    } else {
        pushback_token(tk);
        return left;
    }
}

/**
 * Parse atom expression, where atom expression is:
 *  - a literal number or contanst
 *  - a unary call (sin, cos, ...)
 *  - an expression in parenthesis
 * */
std::shared_ptr<ast::Expression> parse_atom_expression(std::istream& input_stream) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream);
    auto ctx = ast::Context(tk.line, tk.column);
    switch (tk.type) {
    case Token::Type::Real:
        return std::make_shared<ast::RealNumber>(tk.value, ctx);

    case Token::Type::NonNegativeInteger:
        return std::make_shared<ast::NonNegativeInteger>(tk.value, ctx);

    case Token::Type::Keyword_Pi:
        return std::make_shared<ast::EspecialConstant>(ast::EspecialConstant::Pi, ctx);

    case Token::Type::Id:
        return std::make_shared<ast::Variable>(tk.value, ctx);

    case Token::Type::Keyword_Cos:
    case Token::Type::Keyword_Sin:
    case Token::Type::Keyword_Tan:
    case Token::Type::Keyword_Exp:
    case Token::Type::Keyword_Ln:
    case Token::Type::Keyword_Sqrt: {
        Token left_paren = next_token(input_stream);
        if (left_paren.type == Token::Symbol_LeftParen) {
            auto expr = parse_expression(input_stream);
            Token right_paren = next_token(input_stream);
            if (right_paren.type == Token::Type::Symbol_RightParen) {
                if (tk.type == Token::Type::Keyword_Cos)
                    return std::make_shared<ast::UnaryOperation>(ast::UnaryOperation::Cos,
                                                                 std::move(expr),
                                                                 ctx);
                else if (tk.type == Token::Type::Keyword_Sin)
                    return std::make_shared<ast::UnaryOperation>(ast::UnaryOperation::Sin,
                                                                 std::move(expr),
                                                                 ctx);
                else if (tk.type == Token::Type::Keyword_Tan)
                    return std::make_shared<ast::UnaryOperation>(ast::UnaryOperation::Tan,
                                                                 std::move(expr),
                                                                 ctx);
                else if (tk.type == Token::Type::Keyword_Ln)
                    return std::make_shared<ast::UnaryOperation>(ast::UnaryOperation::Ln,
                                                                 std::move(expr),
                                                                 ctx);
                else if (tk.type == Token::Type::Keyword_Exp)
                    return std::make_shared<ast::UnaryOperation>(ast::UnaryOperation::Exp,
                                                                 std::move(expr),
                                                                 ctx);
                else
                    return std::make_shared<ast::UnaryOperation>(ast::UnaryOperation::Sqrt,
                                                                 std::move(expr),
                                                                 ctx);
            } else {
                throw ParserError("expected right parenthesis ')', but found " +
                                      right_paren.value,
                                  right_paren.line, right_paren.column);
            }
        } else {
            throw ParserError(
                    "expected left parenthesis '(' after unary operator, but found " +
                        left_paren.value,
                    left_paren.line, left_paren.column);
        }
    }

    case Token::Type::Symbol_Minus: {
        auto expr = parse_expression(input_stream);
        return std::make_shared<ast::MinusExpression>(std::move(expr), ctx);
    }

    case Token::Type::Symbol_LeftParen: {
        auto expr = parse_expression(input_stream);
        Token right_paren = next_token(input_stream);
        if (right_paren.type != Token::Symbol_RightParen) {
            throw ParserError("expected right parenthesis ')', but found " +
                                  right_paren.value,
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
std::shared_ptr<ast::Expression> parse_add_expression(std::istream& input_stream,
                                                      std::shared_ptr<ast::Expression> left,
                                                      bool minus) {
    using namespace lexer;

    auto _left = std::move(left);
    bool _minus = minus;
    while (true) {
        auto atom_expression = parse_atom_expression(input_stream);
        Token tk = next_token(input_stream);

        std::shared_ptr<ast::Expression> right;
        if (tk.type == Token::Type::Symbol_Times) {
            right = parse_mult_expression(input_stream, std::move(atom_expression), false);
            tk = next_token(input_stream);
        } else if (tk.type == Token::Type::Symbol_Divide) {
            right = parse_mult_expression(input_stream, std::move(atom_expression), true);
            tk = next_token(input_stream);
        } else {
            right = std::move(atom_expression);
        }

        auto ctx = _left->context;

        if (_minus) {
            auto __left = std::move(_left);
            _left = std::make_shared<ast::SubtractionExpression>(std::move(__left),
                                                                 std::move(right),
                                                                 ctx);
        } else {
            auto __left = std::move(_left);
            _left = std::make_shared<ast::AdditionExpression>(std::move(__left),
                                                              std::move(right),
                                                              ctx);
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
std::shared_ptr<ast::Expression> parse_mult_expression(std::istream& input_stream,
                                                       std::shared_ptr<ast::Expression>left,
                                                       bool division) {
    using namespace lexer;

    auto _left = std::move(left);
    auto _division = division;
    while (true) {
        auto right = parse_atom_expression(input_stream);
        auto ctx = _left->context;
        if (_division) {
            auto __left = std::move(_left);
            _left = std::make_shared<ast::DivisionExpression>(std::move(__left),
                                                              std::move(right),
                                                              ctx);
        } else {
            auto __left = std::move(_left);
            _left = std::make_shared<ast::MultiplicationExpression>(std::move(__left),
                                                                    std::move(right),
                                                                    ctx);
        }

        Token tk = lexer::next_token(input_stream);
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
ast::ExpressionList parse_expression_list(std::istream& input_stream) {
    using namespace lexer;

    std::vector<std::shared_ptr<ast::Expression>> expression_list;
    std::shared_ptr<ast::Expression> first = parse_expression(input_stream);
    auto ctx = first->context;
    expression_list.push_back(std::move(first));

    while (true) {
        Token comma = next_token(input_stream);
        if (comma.type != Token::Type::Symbol_Comma) {
            pushback_token(comma);
            break;
        }

        std::shared_ptr<ast::Expression> expr = parse_expression(input_stream);
        expression_list.push_back(std::move(expr));
    }

    return ast::ExpressionList(std::move(expression_list), ctx);
}


/**
 * Parse a list of identifiers:
 * <id> | <id>, <id-list>
 * */
ast::IdentifierList parse_id_list(std::istream& input_stream) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream);
    if (tk.type != Token::Type::Id) {
        throw ParserError("expected an identifier, but found " + tk.value,
                          tk.line, tk.column);
    }

    std::vector<std::string> id_list;
    auto ctx = ast::Context(tk.line, tk.column);
    id_list.push_back(tk.value);

    while (true) {
        Token comma = next_token(input_stream);
        if (comma.type != Token::Type::Symbol_Comma) {
            pushback_token(comma);
            break;
        }
        tk = next_token(input_stream);
        if (tk.type != Token::Type::Id) {
            throw ParserError("expected an identifier, but found " + tk.value,
                              tk.line, tk.column);
        }
        id_list.push_back(tk.value);
    }

    return ast::IdentifierList(id_list, ctx);
}


/**
 * Parse a list that can contain variables and variable indexing's
 * (<id> | <id>[<index>]) | (<id> | <id>[<index>]), <mixed-list>
 * */
ast::MixedList parse_mixed_list(std::istream& input_stream) {
    using namespace lexer;

    ast::Variable first = parse_argument(input_stream);
    std::vector<ast::Variable> mixed_list;
    mixed_list.push_back(first);

    while (true) {
        Token comma = next_token(input_stream);
        if (comma.type != Token::Type::Symbol_Comma) {
            pushback_token(comma);
            break;
        }

        ast::Variable var = parse_argument(input_stream);
        mixed_list.push_back(var);
    }

    return ast::MixedList(mixed_list, first.context);
}


/**
 * An argument is either <identifier> or <identifier>[<index>]
 * */
ast::Variable parse_argument(std::istream& input_stream) {
    using namespace lexer;
    using namespace parser;

    Token identifier = next_token(input_stream);
    auto ctx = ast::Context(identifier.line, identifier.column);
    if (identifier.type != Token::Type::Id) {
        throw ParserError("expected an identifier, but found " + identifier.value,
                          identifier.line, identifier.column);
    }

    Token tk = next_token(input_stream);
    if (tk.type == Token::Type::Symbol_LeftSquareParen) {
        Token index = next_token(input_stream);
        if (index.type != Token::Type::NonNegativeInteger) {
            throw ParserError("expected vector index, but found " + index.value,
                              index.line, index.column);
        }
        tk = next_token(input_stream);
        if (tk.type != Token::Type::Symbol_RightSquareParen) {
            throw ParserError("expected right square bracket ']', but foud " + index.value,
                              index.line, index.column);
        }
        return ast::Variable(identifier.value, std::stoul(index.value), ctx);
    } else {
        pushback_token(tk);
        return ast::Variable(identifier.value, ctx);
    }
}


/**
 * Parse measure operation:
 * measure <arg> -> <arg>;
 * The function expects the measure keyword to have been read.
 * */
ast::MeasureOperation parse_measure_operation(std::istream& input_stream, ast::Context c) {
    using namespace lexer;
    using namespace parser;

    ast::Variable source_arg = parse_argument(input_stream);
    Token tk = next_token(input_stream);
    if (tk.type != Token::Type::Symbol_RightArrow) {
        throw ParserError("expected right arrow '->'", tk.line, tk.column);
    }
    ast::Variable target_arg = parse_argument(input_stream);
    return ast::MeasureOperation(source_arg, target_arg, c);
}


/**
 * Parse a reset operation:
 * reset <arg>;
 * The function expects the reset keyword to have been read.
 * */
ast::ResetOperation parse_reset_operation(std::istream& input_stream, ast::Context c) {
    using namespace lexer;
    using namespace parser;

    ast::Variable target = parse_argument(input_stream);
    return ast::ResetOperation(target, c);
}

/**
 * Parse a unitary operator application:
 * U(<expression_list>) <arg>;
 * CX <arg> <arg>;
 * <operator> <anylist>;
 * <operator> () <anylist>;
 * <operator> (<expression_list>) <anylist>;
 * */
ast::UnitaryOperation parse_unitary_operation(std::istream& input_stream, ast::Context c,
                                              ast::UnitaryOperation::Operator op,
                                              std::string operator_name) {
    using namespace lexer;
    using namespace parser;


    if (op == ast::UnitaryOperation::Operator::U) {
        Token paren = next_token(input_stream);
        if (paren.type != Token::Type::Symbol_LeftParen) {
            throw ParserError("expected left parenthesis '(', but found " + paren.value,
                              paren.line, paren.column);
        }

        ast::ExpressionList param_list = parse_expression_list(input_stream);

        paren = next_token(input_stream);
        if (paren.type != Token::Type::Symbol_RightParen) {
            throw ParserError("expected right parenthesis ')', but found " + paren.value,
                              paren.line, paren.column);
        }

        ast::Variable arg = parse_argument(input_stream);
        ast::MixedList arg_list = ast::MixedList({ arg }, arg.context);

        return ast::UnitaryOperation(op, operator_name, std::move(param_list), arg_list, c);
    } else if (op == ast::UnitaryOperation::Operator::CX) {
        ast::Variable arga = parse_argument(input_stream);
        Token comma = next_token(input_stream);
        if (comma.type != Token::Type::Symbol_Comma) {
            throw ParserError("expected a comma ',', but found " + comma.value,
                             comma.line, comma.column);
        }
        ast::Variable argb = parse_argument(input_stream);
        ast::MixedList arg_list = ast::MixedList({ arga, argb }, arga.context);
        return ast::UnitaryOperation(op, operator_name, std::nullopt, arg_list, c);
    } else {
        Token paren = next_token(input_stream);
        if (paren.type == Token::Type::Symbol_LeftParen) {
            std::optional<ast::ExpressionList> exp_list = std::nullopt;
            paren = next_token(input_stream);
            if (paren.type != Token::Type::Symbol_RightParen) {
                pushback_token(paren);
                exp_list = parse_expression_list(input_stream);
                paren = next_token(input_stream);
                if (paren.type != Token::Type::Symbol_RightParen) {
                    throw ParserError("expected right parenthesis ')', but found " +
                                          paren.value,
                                      paren.line,
                                      paren.column);
                }
            }
            ast::MixedList arg_list = parse_mixed_list(input_stream);
            return ast::UnitaryOperation(op, operator_name, std::move(exp_list),
                                         arg_list, c);
        } else {
            pushback_token(paren);
            ast::MixedList arg_list = parse_mixed_list(input_stream);
            return ast::UnitaryOperation(op, operator_name, std::nullopt, arg_list, c);
        }
    }
}


std::shared_ptr<ast::Statement> parse_quantum_operation(std::istream& input_stream) {
    using namespace lexer;
    using namespace parser;

    Token tk = next_token(input_stream);
    auto context = ast::Context(tk.line, tk.column);

    if (tk.type == Token::Type::Keyword_Measure) {
        auto stmt = parse_measure_operation(input_stream, context);
        return std::make_shared<ast::MeasureOperation>(std::move(stmt));
    } else if (tk.type == Token::Type::Keyword_Reset) {
        auto stmt = parse_reset_operation(input_stream, context);
        return std::make_shared<ast::ResetOperation>(std::move(stmt));
    } else if (tk.type == Token::Type::Keyword_U) {
        auto stmt = parse_unitary_operation(input_stream, context,
                                            ast::UnitaryOperation::Operator::U, "U");
        return std::make_shared<ast::UnitaryOperation>(std::move(stmt));
    } else if (tk.type == Token::Type::Keyword_CX) {
        auto stmt = parse_unitary_operation(input_stream, context,
                                            ast::UnitaryOperation::Operator::CX, "CX");
        return std::make_shared<ast::UnitaryOperation>(std::move(stmt));
    } else if (tk.type == Token::Type::Id) {
        auto stmt = parse_unitary_operation(input_stream, context,
                                            ast::UnitaryOperation::Operator::Defined,
                                            tk.value);
        return std::make_shared<ast::UnitaryOperation>(std::move(stmt));
    } else {
        throw ParserError("expected a quantum operation: reset, measure or unitary",
                          tk.line, tk.column);
    }
}

void read_semicolon(std::istream& input_stream) {
    using namespace lexer;
    using namespace parser;
    Token tk = next_token(input_stream);
    if (tk.type != Token::Symbol_Semicolon) {
        throw ParserError("expected semicolon ';', but found " + tk.value,
                          tk.line, tk.column);
    }
}

}
