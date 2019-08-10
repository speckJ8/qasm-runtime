#ifndef __QASM_LANG__SYMBOL_TABLE_H__
#define __QASM_LANG__SYMBOL_TABLE_H__

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>


namespace qasm_lang {

namespace symbol_table {

    struct Symbol {
    public:
        std::string name;
        std::string type_name;
        unsigned int decl_line;
        unsigned int decl_col;

        Symbol (std::string n, std::string tn, unsigned int dl, unsigned int dc):
            name(n), type_name(tn), decl_line(dl), decl_col(dc) {};

        virtual ~Symbol () = default;
    };

    struct Vector: public Symbol {
    public:
        enum Type { Qbit, Cbit };

        Type type;
        std::string name;
        unsigned int dimension;

        Vector (Type t, std::string n, unsigned int d, unsigned int dl, unsigned int dc):
            Symbol(n, t == Qbit ? "qreg" : "creg", dl, dc), type(t),  name(n), dimension(d)
        {};
    };


    struct Gate: public Symbol {
    public:
        std::string name;
        unsigned int nr_parameters;
        unsigned int nr_arguments;

        Gate (std::string n, unsigned int np, unsigned int na, unsigned int dl, unsigned int dc):
            Symbol(n, "gate", dl, dc), name(n), nr_parameters(np), nr_arguments(na)
        {};
    };


    struct GateParameter: public Symbol {
    public:
        std::string name;
        unsigned int position;

        GateParameter (std::string n, unsigned int p, unsigned int dl, unsigned int dc):
            Symbol(n, "gate parameter", dl, dc), name(n), position(p)
        {};
    };


    void push_new_scope ();
    void pop_scope ();

    void declare_gate (std::string name, unsigned int nr_parameters, unsigned int nr_arguments,
                       unsigned int decl_line, unsigned int decl_col);

    void declare_gate_param (std::string name, unsigned int position, unsigned int decl_line,
                             unsigned int decl_col);

    void declare_qubit (std::string name, unsigned int dimension, unsigned int decl_line,
                        unsigned int decl_col);

    void declare_cbit (std::string name, unsigned int dimension, unsigned int decl_line,
                       unsigned int decl_col);


    /**
     * Check that qubit with name `name` is declared and has dimension at least `indexed_at`
     *
     * If the check is successful returns true and the symbol.
     * If the check is unsuccessful returns false and the symbol identified by `name`, if
     * it is defined.
     * */
    std::tuple<bool, std::optional<qasm_lang::symbol_table::Symbol>>
    check_qubit (std::string name, unsigned int indexed_at=0);

    /**
     * Check that cbit with name `name` is declared and has dimension at least `indexed_at`
     *
     * If the check is successful returns true and the symbol.
     * If the check is unsuccessful returns false and the symbol identified by `name`, if
     * it is defined.
     * */
    std::tuple<bool, std::optional<qasm_lang::symbol_table::Symbol>>
    check_cbit (std::string name, unsigned int indexed_at=0);

    /**
     * Check that gate with name `name` is declared and takes `nr_parameters` parameters
     * and `nr_arguments` arguments
     *
     * If the check is successful returns true and the symbol.
     * If the check is unsuccessful returns false and the symbol identified by `name`, if
     * it is defined.
     * */
    std::tuple<bool, std::optional<qasm_lang::symbol_table::Symbol>>
    check_gate (std::string name, unsigned int nr_parameters, unsigned int nr_arguments);

    std::tuple<bool, std::optional<qasm_lang::symbol_table::Symbol>>
    check_gate_param (std::string name);


    namespace {
        struct Scope {
        public:
            std::unique_ptr<Scope> outer_scope;
            std::unordered_map<std::string, Symbol> symbols;

            Scope () {};
            Scope (std::unique_ptr<Scope> outer): outer_scope(std::move(outer)) {};
        };

        std::unique_ptr<Scope> current_scope;
    };

};

};

#endif // __QASM_LANG__SYMBOL_TABLE_H__
