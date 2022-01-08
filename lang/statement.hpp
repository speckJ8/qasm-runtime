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

#ifndef __QASM_LANG__AST_STMT_H__
#define __QASM_LANG__AST_STMT_H__

#include "expression.hpp"
#include "list.hpp"
#include "context.hpp"
#include "symbol_table.hpp"

#include <optional>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

namespace lang {

class Statement {
public:
    /**
     * Filename, line and column information about where the statement was parsed
     * */
    Context context;

    /**
     * Add variables declared in the statement to the symbol table
     * */
    virtual void declare_symbols() {};

    /**
     * Verify the semantic validity of the statement
     * */
    virtual void verify() {};

    Statement(Context c): context(c) {};

    virtual ~Statement() {};
};


class IfStatement: public Statement {
public:
    Variable variable;
    std::shared_ptr<Expression> target_to_compare;
    std::shared_ptr<Statement> conditional_operation;

    IfStatement(Variable v, std::shared_ptr<Expression> t,
                std::shared_ptr<Statement> qo,
                Context c):
        Statement(c),
        variable(v),
        target_to_compare(std::move(t)),
        conditional_operation(std::move(qo))
    {};

    void declare_symbols() override;
    void verify() override;
};


/**
 * Opaque gate declaration.
 * An opaque gate is a gate that can be physically implemented
 * but whose definition may not be feasible to specify (e.g., because
 * it has parameters that are expensive to estimate).
 * */
class OpaqueDeclaration: public Statement {
public:
    // name of the opaque gate to be declared
    std::string identifier;
    // parameters controlling the operation
    std::optional<IdentifierList> parameters;
    // qubits acted upon
    IdentifierList arguments;

    OpaqueDeclaration(std::string i, std::optional<IdentifierList> p,IdentifierList a,
                      Context c):
       Statement(c), identifier(i), parameters(p), arguments(a)
    {};

    void declare_symbols() override;
};


class VariableDeclaration: public Statement {
public:
    enum DeclarationType { Qbit, Cbit };

    DeclarationType type;
    // name of the variable to be declared
    std::string identifier;
    // dimension of the vector we're declaring
    unsigned int dimension;

    VariableDeclaration(DeclarationType t, std::string i, unsigned int d, Context c):
        Statement(c), type(t), identifier(i), dimension(d)
    {};

    void declare_symbols() override;
};


class GateDeclaration: public Statement {
public:
    std::string identifier;
    // parameters controlling the operation
    std::optional<IdentifierList> parameters;
    // qubits acted upon
    IdentifierList arguments;
    // gate implementation
    std::vector<std::shared_ptr<Statement>> body; 

    GateDeclaration(std::string i, std::optional<IdentifierList> p, IdentifierList a,
                    std::vector<std::shared_ptr<Statement>> b, Context c):
        Statement(c), identifier(i), parameters(p), arguments(a), body(std::move(b))
    {};

    void declare_symbols() override;
    void verify() override;
};


class MeasureOperation: public Statement {
public:
    Variable source;
    Variable target;

    MeasureOperation (Variable s, Variable t, Context c):
        Statement(c), source(s), target(t)
    {};

    void verify() override;
};


class ResetOperation: public Statement {
public:
    Variable target;

    ResetOperation(Variable t, Context c):
        Statement(c), target(t)
    {};

    void verify() override;
};


class UnitaryOperation: public Statement {
public:
    enum Operator {
        /* paremeterized unitary operator */
        U,
        /* controlled not */
        CX,
        /* user defined */
        Defined
    };
    Operator op;
    /* operator name. only useful when `op == Operator::Defined` */
    std::string operator_name;
    std::optional<ExpressionList> expression_list;
    MixedList argument_list;

    UnitaryOperation(Operator o, std::string on, std::optional<ExpressionList> el,
                     MixedList al, Context c):
        Statement(c), op(o), operator_name(on), expression_list(std::move(el)),
        argument_list(al)
    {};

    void verify() override;
};


/**
 * Prevent optimizations across the barrier line of
 * operations involving the variables specified in the barrier.
 * */
class BarrierOperation: public Statement {
public:
    MixedList variables; 

    BarrierOperation(MixedList v, Context c):
        Statement(c), variables(v)
    {};

    void verify() override;
};


class Comment: public Statement {
public:
    std::string comment;
    Comment(std::string comment, Context c): Statement(c), comment(comment) {};
};

}

#endif // __QASM_LANG__AST_STMT_H__
