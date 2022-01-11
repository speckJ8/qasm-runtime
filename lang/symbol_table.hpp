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

#ifndef __QASM_LANG__SYMBOL_TABLE_H__
#define __QASM_LANG__SYMBOL_TABLE_H__

#include <memory>
#include <string>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <iostream>
#include <optional>

namespace lang {

namespace symbol_table {

struct Symbol {
public:
    // name associated with the symbol
    std::string name;
    // name of the symbols declaration type
    std::string type_name;
    // line where the symbol was declared
    size_t decl_line;

    Symbol(std::string n, std::string tn, size_t dl):
        name(n), type_name(tn), decl_line(dl) {};

    virtual ~Symbol() = default;

    virtual std::string to_string() {
        return name + ": " + type_name;
    }
};

struct Vector: public Symbol {
public:
    // we may have a vector of qbits or a vector of classical bits
    enum Type { Qbit, Cbit };

    Type type;
    size_t dimension;

    Vector(Type t, std::string n, size_t d, size_t dl):
        Symbol(n, t == Qbit ? "qreg" : "creg", dl), type(t), dimension(d)
    {};

    std::string to_string() override {
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
    size_t nr_parameters;
    size_t nr_arguments;

    Gate(std::string n, size_t np, size_t na, size_t dl):
        Symbol(n, "gate", dl), nr_parameters(np), nr_arguments(na)
    {};

    std::string to_string() override {
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
    size_t position;

    GateParameter(std::string n, size_t p, size_t dl):
        Symbol(n, "gate-parameter", dl), position(p)
    {};

    std::string to_string() override {
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
    size_t position;

    GateArgument(std::string n, size_t p, size_t dl):
        Symbol(n, "gate-argument", dl), position(p)
    {};

    std::string to_string() override {
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

struct Scope {
private:
    std::string _id;
    std::shared_ptr<Scope> _outer_scope { nullptr };
    std::unordered_map<std::string, std::shared_ptr<Symbol>> _symbols;

    friend void push_scope(std::string);
    friend void pop_scope();
    friend bool restore_scope(std::string);
    friend void declare(std::shared_ptr<Symbol> symbol);
    friend std::optional<std::shared_ptr<Symbol>> get(std::string, bool this_scope);
    friend void dump();
public:
    Scope(std::string id): _id(id) {};
    Scope(std::shared_ptr<Scope> outer, std::string id): _id(id), _outer_scope(outer) {};
};

/**
 * Create new inner scope and move to it.
 * */
void push_scope(std::string name);

/**
 * Pop the current scope and move to its parent scope.
 * */
void pop_scope();

/**
 * Restore a saved scope.
 *
 * @return true if scope was restored and false if scope was not found
 * */
bool restore_scope(std::string name);

/**
 * Declare a new symbol in the current scope.
 * */
void declare(std::shared_ptr<Symbol> symbol);

/**
 * Get a registered symbol
 * If the argument `this_scope` is true the search is only done in the 
 * current scope without checking the outer scopes.
 * */
std::optional<std::shared_ptr<Symbol>> get(std::string name, bool this_scope=false);

/**
 * Print the contents of all the scopes
 * */
void dump();

};

};

#endif // __QASM_LANG__SYMBOL_TABLE_H__
