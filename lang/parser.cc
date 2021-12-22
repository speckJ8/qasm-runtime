#include "parser.hpp"
#include "error.hpp"

#include <cstdlib>
#include <memory>
#include <optional>
#include <fstream>

namespace lang {
namespace parser {

std::string parse_header(Input&);
std::shared_ptr<Statement> parse_statement(Input&);
Variable parse_argument(Input&);
ExpressionList parse_expression_list(Input&);
IdentifierList parse_id_list(Input&);
MixedList parse_mixed_list(Input&);
IfStatement parse_if_statement(Input&, unsigned int, unsigned int);
MeasureOperation parse_measure_operation(Input&, unsigned int, unsigned int);
ResetOperation parse_reset_operation(Input&, unsigned int, unsigned int);
BarrierOperation parse_barrier_operation(Input&, unsigned int, unsigned int);
UnitaryOperation parse_unitary_operation(Input&,
                                         unsigned int,
                                         unsigned int,
                                         UnitaryOperation::Operator,
                                         std::string);
OpaqueDeclaration parse_opaque_declaration(Input&, unsigned int, unsigned int);
GateDeclaration parse_gate_declaration(Input&, unsigned int, unsigned int);
VariableDeclaration parse_variable_declaration(Input&,
                                               unsigned int,
                                               unsigned int,
                                               VariableDeclaration::DeclarationType);
std::shared_ptr<Statement> parse_quantum_operation(Input&);
std::shared_ptr<Expression> parse_expression(Input&);
std::shared_ptr<Expression> parse_mult_expression(Input&);
std::shared_ptr<Expression> parse_pow_expression(Input&);
std::shared_ptr<Expression> parse_atom_expression(Input&);
void read_semicolon(Input&);

Program parse(Input& input) {
    using namespace lexer;

    std::vector<std::shared_ptr<Statement>> statements;
    std::vector<Program> includes;

    parse_header(input);

    Token tk = next_token(input);
    while (tk.type == Token::Type::Keyword_Include) {
        Token include_file = next_token(input);
        if (include_file.type != Token::Type::String) {
            throw Error(input.context(tk.line, tk.col),
                        "expected a string for the name of the include file");
        }
        if (include_file.value == input.filename) {
            throw Error(input.context(tk.line, tk.col),
                        "tried to include file from itself: '" + input.filename + "'");
        }
        read_semicolon(input);
        /*
        Program include = parse(include_file.value);
        includes.push_back(include);
        */
        tk = next_token(input);
    }

    while (tk.type != Token::Type::Eof) {
        pushback_token(tk);
        std::shared_ptr<Statement> stmt = parse_statement(input);
        statements.push_back(stmt);
        tk = next_token(input);
    }

    return Program(input.filename, statements, includes);
}

/**
 * Parse OpenQasm header and return the requested OpenQasm version.
 * */
std::string parse_header(Input& input) {
    using namespace lexer;

    Token tk = next_token(input);
    unsigned int start_line = tk.line;
    if (tk.type != Token::Keyword_OpenQasm) {
        throw Error(input.context(start_line, tk.col), "expected keyword 'OPENQASM'");
    }
    Token version_token = next_token(input);
    if (version_token.type != Token::Real) {
        throw Error(input.context(start_line, tk.col), "expected OpenQasm version number");
    }
    tk = next_token(input);
    if (tk.type != Token::Symbol_Semicolon) {
        throw Error(input.context(start_line, tk.col), "expected semicolon ';'");
    }

    return version_token.value;
}


std::shared_ptr<Statement> parse_statement(Input& input) {
    using namespace lexer;

    Token tk = next_token(input);
    unsigned int start_line = tk.line;
    unsigned int start_col = tk.col;
    switch (tk.type) {
    case Token::Comment: {
        return std::make_shared<Comment>(tk.value, input.context(start_line, start_col));
    }
    case Token::Keyword_If: {
        auto stmt = parse_if_statement(input, start_line, start_col);
        read_semicolon(input);
        return std::make_shared<IfStatement>(stmt);
    }
    case Token::Keyword_Opaque: {
        auto stmt = parse_opaque_declaration(input, start_line, start_col);
        read_semicolon(input);
        return std::make_shared<OpaqueDeclaration>(stmt);
    }
    case Token::Keyword_Barrier: {
        auto stmt = parse_barrier_operation(input, start_line, start_col);
        read_semicolon(input);
        return std::make_shared<BarrierOperation>(stmt);
    }
    case Token::Keyword_Gate: {
        auto stmt = parse_gate_declaration(input, start_line, start_col);
        //read_semicolon(input);
        return std::make_shared<GateDeclaration>(stmt);
    }
    case Token::Keyword_Creg: {
        auto stmt = parse_variable_declaration(input,
                                               start_line,
                                               start_col,
                                               VariableDeclaration::Cbit);
        read_semicolon(input);
        return std::make_shared<VariableDeclaration>(stmt);
    }
    case Token::Keyword_Qreg: {
        auto stmt = parse_variable_declaration(input,
                                               start_line,
                                               start_col,
                                               VariableDeclaration::Qbit);
        read_semicolon(input);
        return std::make_shared<VariableDeclaration>(stmt);
    }
    case Token::Keyword_Measure: {
        auto stmt = parse_measure_operation(input, start_line, start_col);
        read_semicolon(input);
        return std::make_shared<MeasureOperation>(stmt);
    }
    case Token::Keyword_Reset: {
        auto stmt = parse_reset_operation(input, start_line, start_col);
        read_semicolon(input);
        return std::make_shared<ResetOperation>(stmt);
    }
    case Token::Keyword_U: {
        auto stmt = parse_unitary_operation(input,
                                            start_line,
                                            start_col,
                                            UnitaryOperation::Operator::U, "U");
        read_semicolon(input);
        return std::make_shared<UnitaryOperation>(stmt);
    }
    case Token::Keyword_CX: {
        auto stmt = parse_unitary_operation(input,
                                            start_line,
                                            start_col,
                                            UnitaryOperation::Operator::CX, "CX");
        read_semicolon(input);
        return std::make_shared<UnitaryOperation>(stmt);
    }
    case Token::Id: {
        auto stmt = parse_unitary_operation(input,
                                            start_line,
                                            start_col,
                                            UnitaryOperation::Operator::Defined,
                                            tk.value);
        read_semicolon(input);
        return std::make_shared<UnitaryOperation>(stmt);
    }
    case Token::Keyword_Include: {
        throw Error(input.context(start_line, start_col),
                    "include directives must be in the beginning of the file");
    }
    default:
        throw Error(input.context(start_line, start_col), "unexpected token '" + tk.value + "'");
    }
}

/**
 * Parse an if statement:
 * if (<id> == <nninteger>) <quantum_op>.
 * The function expecteds the keyworkd `if` to have been read.
 * */
IfStatement parse_if_statement(Input& input, unsigned int start_line, unsigned int start_col) {
    using namespace lexer;

    Token tk = next_token(input);
    if (tk.type != Token::Type::Symbol_LeftParen) {
        throw Error(input.context(start_line, tk.col), "expected left parenthesis '('");
    }

    Variable variable = parse_argument(input);

    tk = next_token(input);
    if (tk.type != Token::Type::Symbol_EqEq) {
        throw Error(input.context(start_line, tk.col), "expected '=='");
    }

    auto eq_expression = parse_expression(input);

    tk = next_token(input);
    if (tk.type != Token::Type::Symbol_RightParen) {
        throw Error(input.context(start_line, tk.col), "expected ')'");
    }

    auto operation = parse_quantum_operation(input);
    auto context = input.context(start_line, start_col);
    return IfStatement(variable, eq_expression, operation, context);
}


/**
 * Parse an opaque declaration:
 * opaque <id> [(<id-list>)] <id-list>
 * */
OpaqueDeclaration parse_opaque_declaration(Input& input,
                                            unsigned int start_line,
                                            unsigned int start_col)
{
    using namespace lexer;

    std::optional<IdentifierList> param_list = std::nullopt;
    Token name = next_token(input);

    Token paren = next_token(input);
    if (paren.type == Token::Type::Symbol_LeftParen) {
        param_list = parse_id_list(input);
        paren = next_token(input);
        if (paren.type != Token::Type::Symbol_RightParen) {
            throw Error(input.context(start_line, paren.col), "expected right parenthesis ')'");
        }
    } else {
        pushback_token(paren);
    }

    IdentifierList arg_list = parse_id_list(input);
    auto context = input.context(start_line, start_col);
    return OpaqueDeclaration(name.value, param_list, arg_list, context);
}


/**
 * Parse a barrier operation:
 * barrier <any-list>
 * */
BarrierOperation parse_barrier_operation(Input& input,
                                         unsigned int start_line,
                                         unsigned int start_col)
{
    using namespace lexer;

    MixedList mixed_list = parse_mixed_list(input);
    auto context = input.context(start_line, start_col);
    return BarrierOperation(mixed_list, context);
}


/**
 * Parse classical or quantum variable declaration.
 * <creg|qreg> <name>\[<dim>\]
 * */
VariableDeclaration parse_variable_declaration(Input& input,
                                               unsigned int start_line,
                                               unsigned int start_col,
                                               VariableDeclaration::DeclarationType type)
{
    using namespace lexer;

    Token name = next_token(input);
    if (name.type != Token::Type::Id) {
        throw Error(input.context(start_line, name.col),
                    "expected an identifier, but found " + name.value);
    }

    Token paren = next_token(input);
    if (paren.type != Token::Type::Symbol_LeftSquareParen) {
        throw Error(input.context(start_line, paren.col),
                    "expected a left square parenthesis '[', but found " + paren.value);
    }

    Token dimension = next_token(input);
    if (dimension.type != Token::Type::NonNegativeInteger) {
        throw Error(input.context(start_line, dimension.col),
                    "expected the variable dimension, but found " + dimension.value);
    }

    paren = next_token(input);
    if (paren.type != Token::Type::Symbol_RightSquareParen) {
        throw Error(input.context(start_line, paren.col),
                    "expected a right square parenthesis ']', but found " + paren.value);
    }

    auto context = input.context(start_line, start_col);
    return VariableDeclaration(type, name.value, std::stoul(dimension.value), context);
}


GateDeclaration parse_gate_declaration(Input& input,
                                       unsigned int start_line,
                                       unsigned int start_col)
{
    using namespace lexer;

    Token name = next_token(input);
    if (name.type != Token::Type::Id) {
        throw Error(input.context(start_line, name.col),
                    "expected an identifier, but found " + name.value);
    }

    std::optional<IdentifierList> param_list = std::nullopt;
    Token tk = next_token(input);
    if (tk.type == Token::Type::Symbol_LeftParen) {
        param_list = parse_id_list(input);
        tk = next_token(input);
        if (tk.type != Token::Type::Symbol_RightParen) {
            throw Error(input.context(start_line, tk.col),
                        "expected right parenthesis ')', but found " + tk.value);
        }
    } else {
        pushback_token(tk);
    }

    IdentifierList arg_list = parse_id_list(input);

    tk = next_token(input);
    if (tk.type != Token::Type::Symbol_LeftBracket) {
        throw Error(input.context(start_line, tk.col),
                    "expected left bracket '{', but found " + tk.value);
    }

    std::vector<std::shared_ptr<Statement>> operations;
    tk = next_token(input);
    while (tk.type != Token::Type::Symbol_RightBracket) {
        if (tk.type == Token::Type::Keyword_Barrier) {
            auto stmt = parse_barrier_operation(input, tk.line, tk.col);
            operations.push_back(std::make_shared<BarrierOperation>(stmt));
        } else if (tk.type == Token::Type::Keyword_U) {
            auto stmt = parse_unitary_operation(input, tk.line, tk.col,
                                                UnitaryOperation::Operator::U, "U");
            operations.push_back(std::make_shared<UnitaryOperation>(stmt));
        } else if (tk.type == Token::Type::Keyword_CX) {
            auto stmt = parse_unitary_operation(input, tk.line, tk.col,
                                                UnitaryOperation::Operator::CX, "CX");
            operations.push_back(std::make_shared<UnitaryOperation>(stmt));
        } else if (tk.type == Token::Type::Id) {
            auto stmt = parse_unitary_operation(input, tk.line, tk.col,
                                                UnitaryOperation::Operator::Defined,
                                                tk.value);
            operations.push_back(std::make_shared<UnitaryOperation>(stmt));
        } else {
            throw Error(input.context(start_line, tk.col),
                        "expected barrier operation or unitary operation, but found " + tk.value);
        }
        read_semicolon(input);
        tk = next_token(input);
    }

    auto context = input.context(start_line, start_col);
    return GateDeclaration(name.value, param_list, arg_list, operations, context);
}

std::shared_ptr<Expression> parse_expression(Input& input) {
    using namespace lexer;

    std::shared_ptr<Expression> left = parse_mult_expression(input);
    Token tk = next_token(input);
    while (tk.type == Token::Symbol_Plus || tk.type == Token::Symbol_Minus) {
        auto right = parse_mult_expression(input);
        auto ctx = input.context(left->context.start_line, left->context.start_col);
        if (tk.type == Token::Symbol_Minus) {
            left = std::make_shared<SubtractionExpression>(left, right, ctx);
        } else {
            left = std::make_shared<AdditionExpression>(left, right, ctx);
        }
        tk = next_token(input);
    }
    pushback_token(tk);
    return left;
}

std::shared_ptr<Expression> parse_mult_expression(Input& input) {
    using namespace lexer;

    std::shared_ptr<Expression> left = parse_pow_expression(input);
    Token tk = next_token(input);
    while (tk.type == Token::Symbol_Times || tk.type == Token::Symbol_Divide) {
        auto right = parse_pow_expression(input);
        auto ctx = input.context(left->context.start_line, left->context.start_col);
        if (tk.type == Token::Symbol_Divide) {
            left = std::make_shared<DivisionExpression>(left, right, ctx);
        } else {
            left = std::make_shared<MultiplicationExpression>(left, right, ctx);
        }
        tk = next_token(input);
    }
    pushback_token(tk);
    return left;
}

std::shared_ptr<Expression> parse_pow_expression(Input& input) {
    using namespace lexer;

    std::shared_ptr<Expression> left = parse_atom_expression(input);
    Token tk = next_token(input);
    while (tk.type == Token::Symbol_Exp) {
        auto right = parse_atom_expression(input);
        auto ctx = input.context(left->context.start_line, left->context.start_col);
        left = std::make_shared<ExponentiationExpression>(left, right, ctx);
        tk = next_token(input);
    }
    pushback_token(tk);
    return left;
}

/**
 * Parse atom expression, where atom expression is:
 *  - a literal number or contanst
 *  - a unary call (sin, cos, ...)
 *  - an expression in parenthesis
 * */
std::shared_ptr<Expression> parse_atom_expression(Input& input) {
    using namespace lexer;

    Token tk = next_token(input);
    unsigned int start_line = tk.line;
    unsigned int start_col = tk.col;
    switch (tk.type) {
    case Token::Type::Real:
        return std::make_shared<RealNumber>(tk.value, input.context(start_line, start_col));

    case Token::Type::NonNegativeInteger:
        return std::make_shared<NonNegativeInteger>(tk.value, input.context(start_line, start_col));

    case Token::Type::Keyword_Pi:
        return std::make_shared<EspecialConstant>(EspecialConstant::Pi,
                                                  input.context(start_line, start_col));

    case Token::Type::Id:
        return std::make_shared<Variable>(tk.value, input.context(start_line, start_col));

    case Token::Type::Keyword_Cos:
    case Token::Type::Keyword_Sin:
    case Token::Type::Keyword_Tan:
    case Token::Type::Keyword_Exp:
    case Token::Type::Keyword_Ln:
    case Token::Type::Keyword_Sqrt: {
        Token left_paren = next_token(input);
        if (left_paren.type == Token::Symbol_LeftParen) {
            auto expr = parse_expression(input);
            Token right_paren = next_token(input);
            if (right_paren.type == Token::Type::Symbol_RightParen) {
                if (tk.type == Token::Type::Keyword_Cos)
                    return std::make_shared<UnaryOperation>(UnaryOperation::Cos,
                                                            expr,
                                                            input.context(start_line, start_col));
                else if (tk.type == Token::Type::Keyword_Sin)
                    return std::make_shared<UnaryOperation>(UnaryOperation::Sin,
                                                            expr,
                                                            input.context(start_line, start_col));
                else if (tk.type == Token::Type::Keyword_Tan)
                    return std::make_shared<UnaryOperation>(UnaryOperation::Tan,
                                                            expr,
                                                            input.context(start_line, start_col));
                else if (tk.type == Token::Type::Keyword_Ln)
                    return std::make_shared<UnaryOperation>(UnaryOperation::Ln,
                                                            expr,
                                                            input.context(start_line, start_col));
                else if (tk.type == Token::Type::Keyword_Exp)
                    return std::make_shared<UnaryOperation>(UnaryOperation::Exp,
                                                            expr,
                                                            input.context(start_line, start_col));
                else
                    return std::make_shared<UnaryOperation>(UnaryOperation::Sqrt,
                                                            expr,
                                                            input.context(start_line, start_col));
            } else {
                throw Error(input.context(start_line, right_paren.col),
                            "expected right parenthesis ')', but found ");
            }
        } else {
            throw Error(input.context(start_line, left_paren.col),
                        "expected left parenthesis '(' after unary operator, but found " +
                                left_paren.value);
        }
    }

    case Token::Type::Symbol_Minus: {
        auto expr = parse_expression(input);
        return std::make_shared<MinusExpression>(expr, input.context(start_line, start_col));
    }

    case Token::Type::Symbol_LeftParen: {
        auto expr = parse_expression(input);
        Token right_paren = next_token(input);
        if (right_paren.type != Token::Symbol_RightParen) {
            throw Error(input.context(start_line, right_paren.col),
                        "expected right parenthesis ')', but found " + right_paren.value);
        }
        return expr;
    }

    default:
        throw Error(input.context(start_line, tk.col), "unexpected token '" + tk.value + "'");
    }
}


/**
 * Parse an expression list:
 * <expression> | <expression> , <expression-list>
 * */
ExpressionList parse_expression_list(Input& input) {
    using namespace lexer;

    std::vector<std::shared_ptr<Expression>> expression_list;
    std::shared_ptr<Expression> first = parse_expression(input);
    expression_list.push_back(first);

    while (true) {
        Token comma = next_token(input);
        if (comma.type != Token::Type::Symbol_Comma) {
            pushback_token(comma);
            break;
        }

        std::shared_ptr<Expression> expr = parse_expression(input);
        expression_list.push_back(expr);
    }

    auto context = input.context(first->context.start_line, first->context.start_col);
    return ExpressionList(expression_list, context);
}


/**
 * Parse a list of identifiers:
 * <id> | <id>, <id-list>
 * */
IdentifierList parse_id_list(Input& input) {
    using namespace lexer;

    Token tk = next_token(input);
    unsigned int start_line = tk.line;
    unsigned int start_col = tk.col;
    if (tk.type != Token::Type::Id) {
        throw Error(input.context(start_line, tk.col),
                    "expected an identifier, but found " + tk.value);
    }

    std::vector<std::string> id_list;
    id_list.push_back(tk.value);

    while (true) {
        Token comma = next_token(input);
        if (comma.type != Token::Type::Symbol_Comma) {
            pushback_token(comma);
            break;
        }
        tk = next_token(input);
        if (tk.type != Token::Type::Id) {
            throw Error(input.context(start_line, tk.col),
                        "expected an identifier, but found " + tk.value);
        }
        id_list.push_back(tk.value);
    }

    auto context = input.context(start_line, start_col);
    return IdentifierList(id_list, context);
}


/**
 * Parse a list that can contain variables and variable indexing's
 * (<id> | <id>[<index>]) | (<id> | <id>[<index>]), <mixed-list>
 * */
MixedList parse_mixed_list(Input& input) {
    using namespace lexer;

    Variable first = parse_argument(input);
    std::vector<Variable> mixed_list;
    mixed_list.push_back(first);

    while (true) {
        Token comma = next_token(input);
        if (comma.type != Token::Type::Symbol_Comma) {
            pushback_token(comma);
            break;
        }

        Variable var = parse_argument(input);
        mixed_list.push_back(var);
    }

    auto context = input.context(first.context.start_line, first.context.start_col);
    return MixedList(mixed_list, context);
}


/**
 * An argument is either <identifier> or <identifier>[<index>]
 * */
Variable parse_argument(Input& input) {
    using namespace lexer;

    Token identifier = next_token(input);
    unsigned int start_line = identifier.line;
    unsigned int start_col = identifier.col;
    if (identifier.type != Token::Type::Id) {
        throw Error(input.context(start_line, identifier.col),
                    "expected an identifier, but found " + identifier.value);
    }

    Token tk = next_token(input);
    if (tk.type == Token::Type::Symbol_LeftSquareParen) {
        Token index = next_token(input);
        if (index.type != Token::Type::NonNegativeInteger) {
            throw Error(input.context(start_line, index.col),
                        "expected vector index, but found " + index.value);
        }
        tk = next_token(input);
        if (tk.type != Token::Type::Symbol_RightSquareParen) {
            throw Error(input.context(start_line, tk.col),
                        "expected right square bracket ']', but foud " + tk.value);
        }
        auto context = input.context(start_line, start_col);
        return Variable(identifier.value, std::stoul(index.value), context);
    } else {
        pushback_token(tk);
        auto context = input.context(start_line, start_col);
        return Variable(identifier.value, context);
    }
}


/**
 * Parse measure operation:
 * measure <arg> -> <arg>;
 * The function expects the measure keyword to have been read.
 * */
MeasureOperation parse_measure_operation(Input& input,
                                         unsigned int start_line,
                                         unsigned int start_col) {
    using namespace lexer;

    Variable source_arg = parse_argument(input);
    Token tk = next_token(input);
    if (tk.type != Token::Type::Symbol_RightArrow) {
        throw Error(input.context(start_line, tk.col), "expected right arrow '->'");
    }
    auto target_arg = parse_argument(input);
    auto context = input.context(start_line, start_col);
    return MeasureOperation(source_arg, target_arg, context);
}


/**
 * Parse a reset operation:
 * reset <arg>;
 * The function expects the reset keyword to have been read.
 * */
ResetOperation parse_reset_operation(Input& input,
                                     unsigned int start_line,
                                     unsigned int start_col) {
    using namespace lexer;

    Variable target = parse_argument(input);
    auto context = input.context(start_line, start_col);
    return ResetOperation(target, context);
}

/**
 * Parse a unitary operator application:
 * U(<expression_list>) <arg>;
 * CX <arg> <arg>;
 * <operator> <anylist>;
 * <operator> () <anylist>;
 * <operator> (<expression_list>) <anylist>;
 * */
UnitaryOperation parse_unitary_operation(Input& input,
                                         unsigned int start_line,
                                         unsigned int start_col,
                                         UnitaryOperation::Operator op,
                                         std::string operator_name)
{
    using namespace lexer;

    if (op == UnitaryOperation::Operator::U) {
        Token paren = next_token(input);
        if (paren.type != Token::Type::Symbol_LeftParen) {
            throw Error(input.context(start_line, paren.col),
                        "expected left parenthesis '(', but found " + paren.value);
        }

        ExpressionList param_list = parse_expression_list(input);

        paren = next_token(input);
        if (paren.type != Token::Type::Symbol_RightParen) {
            throw Error(input.context(start_line, paren.col),
                        "expected right parenthesis ')', but found " + paren.value);
        }

        Variable arg = parse_argument(input);
        MixedList arg_list = MixedList({ arg }, arg.context);
        auto context = input.context(start_line, start_col);
        return UnitaryOperation(op, operator_name, param_list, arg_list, context);
    } else if (op == UnitaryOperation::Operator::CX) {
        Variable arga = parse_argument(input);
        Token comma = next_token(input);
        if (comma.type != Token::Type::Symbol_Comma) {
            throw Error(input.context(start_line, comma.col),
                        "expected a comma ',', but found " + comma.value);
        }
        Variable argb = parse_argument(input);
        MixedList arg_list = MixedList({ arga, argb }, arga.context);
        auto context = input.context(start_line, start_col);
        return UnitaryOperation(op, operator_name, std::nullopt, arg_list, context);
    } else {
        Token paren = next_token(input);
        if (paren.type == Token::Type::Symbol_LeftParen) {
            std::optional<ExpressionList> exp_list = std::nullopt;
            paren = next_token(input);
            if (paren.type != Token::Type::Symbol_RightParen) {
                pushback_token(paren);
                exp_list = parse_expression_list(input);
                paren = next_token(input);
                if (paren.type != Token::Type::Symbol_RightParen) {
                    throw Error(input.context(start_line, paren.col),
                                "expected right parenthesis ')', but found " + paren.value);
                }
            }
            MixedList arg_list = parse_mixed_list(input);
            auto context = input.context(start_line, start_col);
            return UnitaryOperation(op, operator_name, exp_list, arg_list, context);
        } else {
            pushback_token(paren);
            MixedList arg_list = parse_mixed_list(input);
            auto context = input.context(start_line, start_col);
            return UnitaryOperation(op, operator_name, std::nullopt, arg_list, context);
        }
    }
}


std::shared_ptr<Statement> parse_quantum_operation(Input& input) {
    using namespace lexer;

    Token tk = next_token(input);
    auto start_line = tk.line;
    auto start_col = tk.col;

    if (tk.type == Token::Type::Keyword_Measure) {
        auto stmt = parse_measure_operation(input, start_line, start_col);
        return std::make_shared<MeasureOperation>(stmt);
    } else if (tk.type == Token::Type::Keyword_Reset) {
        auto stmt = parse_reset_operation(input, start_line, start_col);
        return std::make_shared<ResetOperation>(stmt);
    } else if (tk.type == Token::Type::Keyword_U) {
        auto stmt = parse_unitary_operation(input, start_line, start_col,
                                            UnitaryOperation::Operator::U, "U");
        return std::make_shared<UnitaryOperation>(stmt);
    } else if (tk.type == Token::Type::Keyword_CX) {
        auto stmt = parse_unitary_operation(input, start_line, start_col,
                                            UnitaryOperation::Operator::CX, "CX");
        return std::make_shared<UnitaryOperation>(stmt);
    } else if (tk.type == Token::Type::Id) {
        auto stmt = parse_unitary_operation(input, start_line, start_col,
                                            UnitaryOperation::Operator::Defined,
                                            tk.value);
        return std::make_shared<UnitaryOperation>(stmt);
    } else {
        throw Error(input.context(start_line, start_col),
                    "expected a quantum operation: reset, measure or unitary");
    }
}

void read_semicolon(Input& input) {
    using namespace lexer;

    Token tk = next_token(input);
    if (tk.type != Token::Symbol_Semicolon) {
        throw Error(input.context(tk.line, tk.col),"expected semicolon ';', but found " + tk.value);
    }
}

}
}
