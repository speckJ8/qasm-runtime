#ifndef __QASM_LANG__GRAMMAR_H__
#define __QASM_LANG__GRAMMAR_H__


#include "third_party/pegtl.hpp"


namespace qasm {

namespace parser {
    using namespace tao::pegtl;

    struct Semicolon: must<one<';'>> {};
    struct Ws: star<space> {}; // white space

    /**
     * [a-z][A-Za-z0-9_]*
     * */
    struct Identifier: identifier {};

    /**
     * [1-9]+[0-9]* | 0
     * */
    struct NonNegativeInteger: plus<digit> {};

    /**
     * ([0-9]+\.[0-9]*|[0-9]*\.[0-9]+)([eE][-+]?[0-9]+)?
     * */
    struct Real:
        seq<
            // ([0-9]+\.[0-9]*|[0-9]*\.[0-9]+)
            sor<
                seq<plus<digit>, one<'.'>, star<digit>>,
                seq<star<digit>, one<'.'>, plus<digit>>
            >,
            // ([eE][-+]?[0-9]+)?
            opt<seq<sor<one<'e'>, one<'E'>>, opt<sor<one<'-'>, one<'+'>>>, plus<digit>>>
        > {};

    /**
     * \"(\")!\"
     * */
    struct StringLiteral: seq<one<'"'>, star<not_one<'"'>>, one<'"'>> {};

    struct Pi: TAO_PEGTL_STRING("pi") {};

    /**
     * sin | cos | tan | ln | exp | sqrt
     * */
    struct UnaryOp:
        sor<
            TAO_PEGTL_KEYWORD("cos"),
            TAO_PEGTL_KEYWORD("sin"),
            TAO_PEGTL_KEYWORD("tan"),
            TAO_PEGTL_KEYWORD("ln"),
            TAO_PEGTL_KEYWORD("exp"),
            TAO_PEGTL_KEYWORD("sqrt")
        > {};

    /**
     * _Expression includes only non left recursive rules
     * for expressions
     * real | nonnegativeinteger | pi | identifier |
     * -Expression | UnaryOp(Expression) | (Expression)
     * */
    struct Expression;
    struct _Expression:
        sor<
            Real, NonNegativeInteger, Pi, Identifier,
            seq<one<'-'>, Ws, Expression>, // -Expression
            seq<UnaryOp, Ws, one<'('>, Ws, Expression, Ws, one<')'>>, // `op`(Expression)
            seq<one<'('>, Ws, Expression, Ws, one<')'>> // (Expression)
        > {};

    /**
     * Numeric expression
     * */
    struct AddExpression;
    struct MultExpression;
    struct Expression: sor<_Expression, AddExpression, MultExpression> {};

    /**
     * Additive expression without left recursion
     *  + MultExpression _AddExpression
     *  - MultExpression _AddExpression
     * */
    struct _AddExpression:
        sor<
            seq<one<'+'>, Ws, MultExpression, Ws, _AddExpression>,
            seq<one<'-'>, Ws, MultExpression, Ws, _AddExpression>
        > {};
    struct AddExpression: <seq<MultExpression, Ws, _AddExpression> {};

    /**
     * Multiplicative expression without left recursion
     *  * _Expression _MultExpression
     *  / _Expression _MultExpression
     * */
    struct _MultExpression:
        sor<
            seq<one<'*'>, Ws, _Expression, Ws, _MultExpression>,
            seq<one<'/'>, Ws, _Expression, Ws, _MultExpression>
        > {};
    struct MultExpression: seq<_Expression, Ws, _MultExpression> {};

    /**
     * Expression, ExpressionList | Expression
     * */
    struct ExpressionList:
        sor<
            seq<Expression, Ws, one<','>, Ws, ExpressionList>,
            Expression
        >
    {};

    /**
     * Argument to an operator
     * Indentifier[NonNegativeInteger] |
     * Identifier 
     * */
    struct Argument:
        sor<
            seq<Identifier, Ws, one<'['>, Ws, NonNegativeInteger, Ws, one<']'>>,
            Identifier
        > {};

    /**
     * Identifier | Identifier, IdList
     * */
    struct IdList: sor<seq<Identifier, Ws, one<','>, Ws, IdList>, Identifier> {};

    /**
     * Identifier, MixedList |
     * Identifier[NonNegativeInteger], MixedList |
     * Identifier[NonNegativeInteger], IdList |
     * Identifier[NonNegativeInteger]
     * */
    struct MixedList:
        sor<
            seq<Identifier, Ws, one<','>, Ws, MixedList>,
            seq<
                seq<Identifier, Ws, one<'['>, Ws, NonNegativeInteger, Ws, one<']'>>,
                Ws,
                one<','>,
                Ws,
                MixedList
            >,
            seq<
                seq<Identifier, Ws, one<'['>, Ws, NonNegativeInteger, Ws, one<']'>>,
                Ws, 
                one<','>,
                Ws,
                IdList
            >,
            seq<Identifier, Ws, one<'['>, Ws, NonNegativeInteger, Ws, one<']'>>
        > {};

    struct AnyList: sor<MixedList, IdList> {};

    /**
     * U(ExpressionList) Argument; |
     * CX Argument, Argument; |
     * Identifier AnyList; |
     * Identifier () AnyList; |
     * Identifier (ExpressionList) AnyList;
     * */
    struct UnitaryOperation:
        sor<
            seq<
                one<'U'>,
                Ws,
                one<'('>,
                Ws,
                ExpressionList,
                Ws,
                one<')'>,
                Ws,
                Argument,
                Ws,
                Semicolon
            >,
            seq<
                TAO_PEGTL_STRING("CX"),
                Ws,
                Argument,
                Ws,
                one<','>,
                Ws,
                Argument,
                Ws,
                Semicolon
            >,
            seq<
                Identifier,
                Ws,
                opt<one<'('>, opt<ExpressionList>, one<')'>>,
                Ws,
                AnyList,
                Ws,
                Semicolon
            >
        > {};

    /**
     * barrier AnyList;
     * */
    struct BarrierOperation:
        seq<TAO_PEGTL_STRING("barrier"), Ws, AnyList, Ws, Semicolon> {};

    /**
     * The following three rules describe this rule (but without left recursion):
     * UnitaryOperation |
     * barrier IdList; |
     * GOperationList UnitaryOperation |
     * GOperationList barrier IdList;
     * */
    struct GOperationList:
        sor<
            seq<UnitaryOperation, Ws, GOperationList>,
            seq<BarrierOperation, Ws, GOperationList>,
            UnitaryOperation,
            BarrierOperation
        > {};

    /**
     * gate Identifier IdList { |
     * gate Identifier () IdList { |
     * gate Identifier (IdList) IdList {
     * */
    struct GateDeclaration:
        seq<
            TAO_PEGTL_STRING("gate"),
            Ws,
            must<Identifier>,
            Ws,
            opt<seq<one<'('>, opt<IdList>, one<')'>>>,
            Ws,
            IdList,
            Ws,
            one<'{'>,
            Ws,
            opt<GOperationList>,
            Ws,
            one<'}'>
        > {};

    /**
     * qreg Identifier [NonNegativeInteger] |
     * creg Identifier [NonNegativeInteger]
     * */
    struct Declaration:
        seq<
            sor<TAO_PEGTL_STRING("qreg"), TAO_PEGTL_STRING("creg")>,
            Ws,
            Identifier,
            Ws,
            one<'['>,
            Ws,
            NonNegativeInteger,
            Ws,
            one<']'>,
            Ws,
            Semicolon
        > {};

    /**
     * measure Argument -> Argument; |
     * */
    struct MeasureOperation:
        seq<
            TAO_PEGTL_STRING("measure"),
            Ws,
            Argument,
            Ws,
            TAO_PEGTL_STRING("->"),
            Ws,
            Argument,
            Ws,
            Semicolon
        > {};

    /**
     * reset Argument;
     * */
    struct ResetOperation: seq<TAO_PEGTL_STRING("reset"), Ws, Argument, Ws, Semicolon> {};

    /**
     * UnitaryOperation | MeasureOperation | ResetOperation
     * */
    struct QuantumOperation:
        sor<
            MeasureOperation,
            ResetOperation,
            UnitaryOperation
        > {};

    /**
     * opaque Identifier IdList ; |
     * opaque Identifier () IdList; |
     * opaque Identifier (IdList) IdList;
     * */
    struct OpaqueDeclaration:
        seq<
            TAO_PEGTL_STRING("opaque"),
            Ws,
            Identifier,
            Ws,
            opt<one<'('>, opt<IdList>, one<')'>>,
            Ws,
            IdList,
            Ws,
            Semicolon
        > {};

    /**
     * if (Identifier == NonNegativeInteger) QuantumOperation |
     * */
    struct IfOperation:
        seq<
            TAO_PEGTL_STRING("if"),
            Ws,
            one<'('>,
            Ws,
            Identifier,
            Ws,
            TAO_PEGTL_STRING("=="),
            Ws,
            Expression,
            Ws,
            one<')'>,
            Ws,
            QuantumOperation
        > {};

    /**
     * include StringLiteral;
     * */
    struct Include: seq<TAO_PEGTL_STRING("include"), Ws, StringLiteral, Ws, Semicolon> {};

    /**
     * IfOperation | BarrierOperation | OpaqueDeclaration |
     * Declaration |
     * GateDeclarataion |
     * Include |
     * QuantumOperation
     * */
    struct Statement:
        sor<
            IfOperation,
            OpaqueDeclaration,
            BarrierOperation,
            // Include,
            Declaration,
            GateDeclaration,
            QuantumOperation
        > {};

    struct Statements: sor<seq<Statement, Ws, Statements>, Statement> {};

    /**
     * OPENQASM Real; Program
     * */
    struct MainProgram:
        must<
            Ws,
            TAO_PEGTL_STRING("OPENQASM"),
            Ws,
            Real,
            Ws,
            one<';'>,
            Ws,
            Statements,
            Ws,
            eolf
        > {};

};

};


#endif // __QASM_LANG__GRAMMAR_H__
