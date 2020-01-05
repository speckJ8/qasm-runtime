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
        // name associated with the symbol
        std::string name;
        // name of the symbols declaration type
        std::string type_name;
        // line where the symbol was declared
        unsigned int decl_line;

        Symbol (std::string n, std::string tn, unsigned int dl):
            name(n), type_name(tn), decl_line(dl) {};

        virtual ~Symbol () = default;
    };

    struct Vector: public Symbol {
    public:
        // we may have a vector of qbits or a vector of classical bits
        enum Type { Qbit, Cbit };

        Type type;
        unsigned int dimension;

        Vector (Type t, std::string n, unsigned int d, unsigned int dl):
            Symbol(n, t == Qbit ? "qreg" : "creg", dl), type(t), dimension(d)
        {};
    };


    struct Gate: public Symbol {
    public:
        unsigned int nr_parameters;
        unsigned int nr_arguments;

        Gate (std::string n, unsigned int np, unsigned int na, unsigned int dl):
            Symbol(n, "gate", dl), nr_parameters(np), nr_arguments(na)
        {};
    };


    struct GateParameter: public Symbol {
    public:
        unsigned int position;

        GateParameter (std::string n, unsigned int p, unsigned int dl):
            Symbol(n, "gate parameter", dl), position(p)
        {};
    };

    /**
     * Create new inner scope and move to it.
     * */
    void push_new_scope ();
    /**
     * Destroy current scope and move to parent scope.
     * */
    void pop_scope ();
    /**
     * Pop the current scope and store it so that is can be restored
     * later.
     * @param name the name to use to save the scope
     * */
    void save_scope (std::string name);
    /**
     * Restore a saved scope
     * */
    void restore_scope (std::string name);

    void declare_gate (std::string name, unsigned int nr_parameters,
                       unsigned int nr_arguments, unsigned int decl_line);

    void declare_gate_param (std::string name, unsigned int position,
                             unsigned int decl_line);

    void declare_qubit_vector (std::string name, unsigned int dimension,
                               unsigned int decl_line);

    void declare_cbit_vector (std::string name, unsigned int dimension,
                              unsigned int decl_line);

    /**
     * Check that qubit vector with name `name` is declared and has dimension at 
     * least `indexed_at`+1
     *
     * If the check is successful returns true and the symbol.
     * If the check is unsuccessful returns false and the symbol identified by `name`, if
     * it is defined.
     * */
    std::tuple<bool, std::optional<qasm_lang::symbol_table::Symbol>>
    check_qubit_vector (std::string name, unsigned int indexed_at=0);

    /**
     * Check that cbit vector with name `name` is declared and has dimension at 
     * least `indexed_at`+1
     *
     * If the check is successful returns true and the symbol.
     * If the check is unsuccessful returns false and the symbol identified by `name`, if
     * it is defined.
     * */
    std::tuple<bool, std::optional<qasm_lang::symbol_table::Symbol>>
    check_cbit_vector (std::string name, unsigned int indexed_at=0);

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
        std::unordered_map<std::string, Scope> saved_scopes;
    };

    /**
     * Print the contents 
     * */
    void dump ();

};

};

#endif // __QASM_LANG__SYMBOL_TABLE_H__
