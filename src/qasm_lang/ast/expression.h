#ifndef __QASM_LANG__AST_EXPRESSION_H__
#define __QASM_LANG__AST_EXPRESSION_H__

#include <string>

namespace qasm_lang {

namespace ast {

    class Expression {};


    class RealNumber: Expression {
    private:
        std::string representation;

    public:
        RealNumber (std::string r): representation(r) {};
    };


    class NonNegativeInteger: Expression {
    private:
        std::string representation;

    public:
        NonNegativeInteger (std::string r): representation(r) {};
    };


    class EspecialConstant: Expression {
    public:
        enum Name { Pi };

    private:
        Name name;

    public:
        EspecialConstant (Name n): name(n) {};
    };


    class AdditionExpression: Expression {
    private:
        Expression left;
        Expression right;

    public:
        AdditionExpression (Expression l, Expression r): left(l), right(r) {};
    };


    class MultiplicationExpression: Expression {
    private:
        Expression left;
        Expression right;

    public:
        MultiplicationExpression (Expression l, Expression r): left(l), right(r) {};
    };


    class SubtractionExpression: Expression {
    private:
        Expression left;
        Expression right;

    public:
        SubtractionExpression (Expression l, Expression r): left(l), right(r) {};
    };


    class DivisionExpression: Expression {
    private:
        Expression left;
        Expression right;

    public:
        DivisionExpression (Expression l, Expression r): left(l), right(r) {};
    };


    class ExponentiationExpression: Expression {
    private:
        Expression left;
        Expression right;

    public:
        ExponentiationExpression (Expression l, Expression r): left(l), right(r) {};
    };


    class UnaryOperation: Expression {
    public:
        enum UnaryOp { Sin, Cos, Tan, Exp, Ln, Sqrt };
    private:
        UnaryOp operation;
        Expression target;

    public:
        UnaryOperation (UnaryOp o, Expression t): operation(o), target(t) {};
    };


};

};

#endif // __QASM_LANG__AST_EXPRESSION_H__
