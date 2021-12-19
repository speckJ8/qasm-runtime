#ifndef __QASM_LANG__AST_EXPRESSION_H__
#define __QASM_LANG__AST_EXPRESSION_H__

#include <string>
#include <optional>
#include <vector>
#include <memory>
#include <iostream>

#include "context.h"

namespace qasm {

namespace ast {

class Expression {
public:
    /**
     * Return true if the expression is atomic (a value) and false if
     * it is a vector
     * */
    virtual bool is_atom() {
        return true;
    }

    /**
     * Check if the expression is a qubit or a classical variable
     * */
    virtual bool is_qubit() {
        return false;
    }

    /**
     * Line, column and filename information about where the node was
     * parsed
     * */
    Context context;

    Expression(Context c): context(c) {};

    /**
     * Verify the semantic validity of the expression
     * */
    virtual void verify() {};

    /**
     * Return the represenation for the expression
     * */
    virtual std::string to_string() {
        return "";
    };

    virtual ~Expression() {};
};


class RealNumber: public Expression {
public:
    std::string representation;

    RealNumber(std::string r, Context c): Expression(c), representation(r) {};

    std::string to_string() override {
        return representation;
    }

    double value () {
        return std::stod(representation);
    }
};


class NonNegativeInteger: public Expression {
public:
    std::string representation;

    NonNegativeInteger(std::string r, Context c): Expression(c), representation(r) {};

    std::string to_string() override {
        return representation;
    }

    unsigned long value() {
        return std::stoul(representation);
    }
};


class EspecialConstant: public Expression {
public:
    enum Name { Pi };
    Name name;

    EspecialConstant(Name n, Context c): Expression(c), name(n) {};

    std::string to_string() override {
        switch (name) {
        case Pi: return "pi";
        default: return "pi";
        }
    }
};


class Variable: public Expression {
public:
    std::string identifier;
    // in case the variable is a vector and we're indexing it
    std::optional<unsigned int> index;

    Variable(std::string i, Context c):
         Expression(c), identifier(i), index(std::nullopt)
    {};
    Variable(std::string i, unsigned int _i, Context c):
        Expression(c), identifier(i), index(_i)
    {};

    bool is_atom() override {
        return !index.has_value();
    }

    bool is_qubit() override;

    void verify() override;

    std::string to_string() override {
        if (index) {
            return identifier + "[" + std::to_string(index.value()) + "]";
        } else {
            return identifier;
        }
    }
};


class MinusExpression: public Expression {
public:
    std::shared_ptr<Expression> negated_expression;

    MinusExpression(std::shared_ptr<Expression> e, Context c):
        Expression(c), negated_expression(std::move(e))
    {};

    void verify() override;

    std::string to_string() override {
        return "-" + negated_expression->to_string();
    }
};


class AdditionExpression: public Expression {
public:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;

    AdditionExpression(std::shared_ptr<Expression> l, std::shared_ptr<Expression> r,
                        Context c):
        Expression(c), left(std::move(l)), right(std::move(r))
    {};

    void verify() override;

    std::string to_string() override {
        return "(" + left->to_string() + " + " + right->to_string() + ")";
    }
};


class MultiplicationExpression: public Expression {
public:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;

    MultiplicationExpression(std::shared_ptr<Expression> l,
                             std::shared_ptr<Expression> r,
                             Context c):
        Expression(c), left(std::move(l)), right(std::move(r))
    {};

    void verify() override;

    std::string to_string() override {
        return "(" + left->to_string() + " * " + right->to_string() + ")";
    }
};


class SubtractionExpression: public Expression {
public:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;

    SubtractionExpression(std::shared_ptr<Expression> l,std::shared_ptr<Expression> r,
                          Context c):
        Expression(c), left(std::move(l)), right(std::move(r))
    {};

    void verify() override;

    std::string to_string() override {
        return "(" + left->to_string() + " - " + right->to_string() + ")";
    }
};


class DivisionExpression: public Expression {
public:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;

    DivisionExpression(std::shared_ptr<Expression> l, std::shared_ptr<Expression> r,
                       Context c):
        Expression(c), left(std::move(l)), right(std::move(r))
    {};

    void verify() override;

    std::string to_string() override {
        return "(" + left->to_string() + " / " + right->to_string() + ")";
    }
};


class ExponentiationExpression: public Expression {
public:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;

    ExponentiationExpression(std::shared_ptr<Expression> l,
                             std::shared_ptr<Expression> r,
                             Context c):
        Expression(c), left(std::move(l)), right(std::move(r))
    {};

    void verify() override;

    std::string to_string() override {
        return "(" + left->to_string() + "^" + right->to_string() + ")";
    }
};


class UnaryOperation: public Expression {
public:
    enum UnaryOp { Sin, Cos, Tan, Exp, Ln, Sqrt };
    UnaryOp operation;
    std::shared_ptr<Expression> target;

public:
    UnaryOperation(UnaryOp o, std::shared_ptr<Expression> t, Context c):
        Expression(c), operation(o), target(std::move(t))
    {};

    void verify() override;

    std::string to_string() override {
        switch (operation) {
        case Sin : return "sin("  + target->to_string() + ")";
        case Cos : return "cos("  + target->to_string() + ")";
        case Tan : return "tan("  + target->to_string() + ")";
        case Exp : return "exp("  + target->to_string() + ")";
        case Ln  : return "ln("   + target->to_string() + ")";
        case Sqrt: return "sqrt(" + target->to_string() + ")";
        default  : return "x("    + target->to_string() + ")";
        }
    }
};
};

};

#endif // __QASM_LANG__AST_EXPRESSION_H__
