#ifndef __QASM_LANG__AST_EXPRESSION_H__
#define __QASM_LANG__AST_EXPRESSION_H__

#include <string>
#include <optional>
#include <vector>

namespace qasm_lang {

namespace ast {

    class Expression {};


    class RealNumber: public Expression {
    public:
        std::string representation;

        RealNumber (std::string r): representation(r) {};
    };


    class NonNegativeInteger: public Expression {
    public:
        std::string representation;

        NonNegativeInteger (std::string r): representation(r) {};
    };


    class EspecialConstant: public Expression {
    public:
        enum Name { Pi };
        Name name;

        EspecialConstant (Name n): name(n) {};
    };


    class Variable: public Expression {
    public:
        std::string identifier;
        // in case the variable is a vector and we're indexing it
        std::optional<unsigned int> index;

        Variable (std::string i): identifier(i), index({}) {};
        Variable (std::string i, unsigned int _i): identifier(i), index(_i) {};

        inline std::string name () { return identifier; };
    };


    class MinusExpression: public Expression {
    public:
        Expression negated_expression;

        MinusExpression (Expression e): negated_expression(e) {};
    };


    class AdditionExpression: public Expression {
    public:
        Expression left;
        Expression right;

        AdditionExpression (Expression l, Expression r): left(l), right(r) {};
    };


    class MultiplicationExpression: public Expression {
    public:
        Expression left;
        Expression right;

        MultiplicationExpression (Expression l, Expression r): left(l), right(r) {};
    };


    class SubtractionExpression: public Expression {
    public:
        Expression left;
        Expression right;

        SubtractionExpression (Expression l, Expression r): left(l), right(r) {};
    };


    class DivisionExpression: public Expression {
    public:
        Expression left;
        Expression right;

        DivisionExpression (Expression l, Expression r): left(l), right(r) {};
    };


    class ExponentiationExpression: public Expression {
    public:
        Expression left;
        Expression right;

        ExponentiationExpression (Expression l, Expression r): left(l), right(r) {};
    };


    class UnaryOperation: public Expression {
    public:
        enum UnaryOp { Sin, Cos, Tan, Exp, Ln, Sqrt };
        UnaryOp operation;
        Expression target;

    public:
        UnaryOperation (UnaryOp o, Expression t): operation(o), target(t) {};
    };
};

};

#endif // __QASM_LANG__AST_EXPRESSION_H__
