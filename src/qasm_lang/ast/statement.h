#ifndef __QASM_LANG__AST_STMT_H__
#define __QASM_LANG__AST_STMT_H__

#include <optional>
#include <string>
#include <vector>

#include "expression.h"
#include "variable.h"

namespace qasm_lang {

namespace ast {

    class Statement {};
    class QuantumOperation: Statement {};


    class IfStatement: Statement {
    private:
        Variable variable;
        Expression target_to_compare;
        std::vector<Statement> conditional_block;

    public:
        IfStatement (Variable v, Expression t, std::vector<Statement> c):
            variable(v),
            target_to_compare(t),
            conditional_block(c)
        {};
    };


    /**
     * Opaque gate declaration.
     * An opaque gate is a gate that can be physically implemented
     * but whose definition may not be feasible to specify (e.g., because
     * it has parameters that are expensive to estimate).
     * */
    class OpaqueDeclaration: Statement {
    private:
        // name of the opaque gate to be declared
        std::string identifier;
        // parameters controlling the operation
        std::optional<std::vector<std::string>> parameters;
        // qubits acted upon
        std::vector<std::string> arguments;
    };


    class VariableDeclaration: Statement {
    public:
        enum DeclarationType { Qbit, Cbit };

    private:
        DeclarationType type;
        // name of the variable to be declared
        std::string identifier;
        // dimension of the vector we're declaring
        unsigned int dimension;
    };


    /**
     * Quantum operations that can appear inside a
     * gate declaration body
     * */
    class GateDeclarationOperation: Statement {};

    class GateDeclaration: Statement {
    private:
        std::string identifier;
        // parameters controlling the operation
        std::optional<std::vector<std::string>> parameters;
        // qubits acted upon
        std::vector<std::string> arguments;
        // gate implementation
        std::vector<GateDeclarationOperation> body; 
    };


    class MeasureOperation: QuantumOperation, GateDeclaration {
    private:
        Variable source;
        Variable target;
    };


    class ResetOperation: QuantumOperation, GateDeclaration {
    private:
        Variable target;
    };


    class UnitaryOperation: QuantumOperation, GateDeclaration {
    private:
        std::string gate;
        std::optional<Expression> parameters;
        std::vector<Variable> arguments;
    };


    /**
     * Prevent optimizations across the barrier line of
     * operations involving the variables specified in the barrier.
     * */
    class BarrierOperation: GateDeclarationOperation {
    private:
        std::vector<Variable> variables; 
    };


};

};

#endif // __QASM_LANG__AST_STMT_H__
