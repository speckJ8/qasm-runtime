#ifndef __QASM_LANG__AST_STMT_H__
#define __QASM_LANG__AST_STMT_H__

#include <optional>
#include <string>
#include <vector>

#include "expression.h"
#include "list.h"
#include "qasm_lang/symbol_table.h"

namespace qasm_lang {

namespace ast {

    class Statement {};
    class QuantumOperation: virtual public Statement {};


    class IfStatement: public Statement {
    public:
        Variable variable;
        Expression target_to_compare;
        QuantumOperation conditional_operation;

        IfStatement (Variable v, Expression t, QuantumOperation qo):
            variable(v),
            target_to_compare(t),
            conditional_operation(qo)
        {};
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
        OpaqueDeclaration (std::string i, std::optional<IdentifierList> p, IdentifierList a,
                           unsigned int line):
           identifier(i), parameters(p), arguments(a)
        {
            unsigned int nr_params = 0;
            if (parameters.has_value())
                nr_params = parameters.value().id_list.size();
            unsigned int nr_args = arguments.id_list.size();
            symbol_table::declare_gate(i, nr_params, nr_args, line);
        }; 
    };


    class VariableDeclaration: public Statement {
    public:
        enum DeclarationType { Qbit, Cbit };

        DeclarationType type;
        // name of the variable to be declared
        std::string identifier;
        // dimension of the vector we're declaring
        unsigned int dimension;

        VariableDeclaration (DeclarationType t, std::string i, unsigned int d,
                             unsigned int line):
            type(t), identifier(i), dimension(d)
        {
            if (t == Qbit) {
                symbol_table::declare_qubit_vector(i, d, line);
            } else {
                symbol_table::declare_cbit_vector(i, d, line);
            }
        };
    };


    /**
     * Quantum operations that can appear inside a
     * gate declaration body
     * */
    class GateDeclarationOperation: virtual public Statement {};


    class GateDeclaration: public Statement {
    public:
        std::string identifier;
        // parameters controlling the operation
        std::optional<IdentifierList> parameters;
        // qubits acted upon
        IdentifierList arguments;
        // gate implementation
        std::vector<GateDeclarationOperation> body; 
        GateDeclaration (std::string i, std::optional<IdentifierList> p, IdentifierList a,
                         std::vector<GateDeclarationOperation> b, unsigned int line):
            identifier(i), parameters(p), arguments(a), body(b)
        {
            unsigned int nr_params = 0;
            if (parameters.has_value())
                nr_params = parameters.value().id_list.size();
            unsigned int nr_args = arguments.id_list.size();
            symbol_table::declare_gate(i, nr_params, nr_args, line);
        };
    };


    class MeasureOperation: public QuantumOperation, public GateDeclarationOperation {
    public:
        Variable source;
        Variable target;
        MeasureOperation (Variable s, Variable t): source(s), target(t) {};
    };


    class ResetOperation: public QuantumOperation, public GateDeclarationOperation {
    public:
        Variable target;
        ResetOperation (Variable t): target(t) {};
    };


    class UnitaryOperation: public QuantumOperation, public GateDeclarationOperation {
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
        UnitaryOperation (Operator o, std::string on, std::optional<ExpressionList> el,
                          MixedList al):
            op(o), operator_name(on), expression_list(el), argument_list(al)
        {};
    };


    /**
     * Prevent optimizations across the barrier line of
     * operations involving the variables specified in the barrier.
     * */
    class BarrierOperation: public GateDeclarationOperation {
    public:
         MixedList variables; 
        BarrierOperation (MixedList v): variables(v) {};
    };


};

};

#endif // __QASM_LANG__AST_STMT_H__
