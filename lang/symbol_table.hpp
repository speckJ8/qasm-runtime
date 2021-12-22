#ifndef __QASM_LANG__SYMBOL_TABLE_H__
#define __QASM_LANG__SYMBOL_TABLE_H__

#include <memory>
#include <string>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <iostream>

namespace lang {

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

    virtual std::string to_string () {
        return name + ": " + type_name;
    }
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

    std::string to_string () override {
        std::stringstream ss;
        std::stringstream type;
        type << type_name << "[" << dimension << "]";
        ss << std::setw(10) << name
           << "  |  "
           << "type: " << std::setw(18) << type.str()
           << "  |  "
           << "declared_at: " << std::setw(3) << decl_line;
        return ss.str();
    }
};


struct Gate: public Symbol {
public:
    unsigned int nr_parameters;
    unsigned int nr_arguments;

    Gate (std::string n, unsigned int np, unsigned int na, unsigned int dl):
        Symbol(n, "gate", dl), nr_parameters(np), nr_arguments(na)
    {};

    std::string to_string () override {
        std::stringstream ss;
        std::stringstream type;
        type << type_name << "{" << nr_parameters << ", " << nr_arguments << "}";
        ss << std::setw(10) << name
           << "  |  "
           << "type: " << std::setw(18) << type.str()
           << "  |  "
           << "declared_at: " << std::setw(3) << decl_line;
        return ss.str();
    }
};


struct GateParameter: public Symbol {
public:
    unsigned int position;

    GateParameter (std::string n, unsigned int p, unsigned int dl):
        Symbol(n, "gate-parameter", dl), position(p)
    {};

    std::string to_string () override {
        std::stringstream ss;
        std::stringstream type;
        type << type_name << "{" << position << "}";
        ss << std::setw(10) << name
           << "  |  "
           << "type: " << std::setw(18) << type.str()
           << "  |  "
           << "declared_at: " << std::setw(3) << decl_line;
        return ss.str();
    }
};

struct GateArgument: public Symbol {
    unsigned int position;

    GateArgument (std::string n, unsigned int p, unsigned int dl):
        Symbol(n, "gate-argument", dl), position(p)
    {};

    std::string to_string () override {
        std::stringstream ss;
        std::stringstream type;
        type << type_name << "{" << position << "}";
        ss << std::setw(10) << name
           << "  |  "
           << "type: " << std::setw(18) << type.str()
           << "  |  "
           << "declared_at: " << std::setw(3) << decl_line;
        return ss.str();
    }
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
 *
 * @param name the name to use to save the scope
 * */
void pop_and_save_scope (std::string name);
/**
 * Restore a saved scope
 *
 * @return true if scope was restored and false if scope was not found
 * */
bool restore_scope (std::string name);

/**
 * Declare a new symbol, identified by symbol.name
 * */
void declare (std::shared_ptr<Symbol> symbol);

/**
 * Get a registered symbol
 * If the argument `this_scope` is true the search is only done in the 
 * current scope without checking the parent scope
 * */
std::optional<std::shared_ptr<Symbol>> get (std::string name, bool this_scope=false);

namespace {
    struct Scope;
    struct Scope {
    public:
        std::shared_ptr<Scope> outer_scope;
        std::unordered_map<std::string, std::shared_ptr<Symbol>> symbols;
        std::unordered_map<std::string, std::shared_ptr<Scope>> saved_scopes;

        Scope () {};
        Scope (std::shared_ptr<Scope> outer): outer_scope(outer) {};
    };
};

/**
 * Print the contents 
 * */
void dump ();

};

};

#endif // __QASM_LANG__SYMBOL_TABLE_H__
