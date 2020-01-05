#include "symbol_table.h"
#include <cassert>


void qasm_lang::symbol_table::push_new_scope () {
    if (current_scope == nullptr) {
        current_scope = std::make_unique<Scope>();
    } else {
        current_scope = std::make_unique<Scope>(std::move(current_scope));
    }
}


void qasm_lang::symbol_table::pop_scope () {
    if (current_scope != nullptr) {
        std::unique_ptr<Scope> outer_scope = std::move(current_scope->outer_scope);
        current_scope.reset();
        current_scope = std::move(outer_scope);
    }
}


void qasm_lang::symbol_table::declare_qubit_vector (std::string name,
                                                    unsigned int dimension,
                                                    unsigned int decl_line) {
    assert(current_scope != nullptr);
    current_scope->symbols.insert(
        { name, Vector(Vector::Type::Qbit, name, dimension, decl_line) });
}


void qasm_lang::symbol_table::declare_cbit_vector (std::string name,
                                                   unsigned int dimension,
                                                   unsigned int decl_line) {
    assert(current_scope != nullptr);
    current_scope->symbols.insert(
        { name, Vector(Vector::Type::Cbit, name, dimension, decl_line) });
}


void qasm_lang::symbol_table::declare_gate_param (std::string name,
                                                  unsigned int position,
                                                  unsigned int decl_line) {
    assert(current_scope != nullptr);
    current_scope->symbols.insert(
        { name,  GateParameter(name, position, decl_line) });
}


void qasm_lang::symbol_table::declare_gate (std::string name,
                                            unsigned int nr_parameters,
                                            unsigned int nr_arguments,
                                            unsigned int decl_line) {
    assert(current_scope != nullptr);
    current_scope->symbols.insert(
        { name, Gate(name, nr_parameters, nr_arguments, decl_line) });
}


std::tuple<bool, std::optional<qasm_lang::symbol_table::Symbol>>
qasm_lang::symbol_table::check_qubit_vector (std::string name, unsigned int indexed_at) {
    assert(current_scope != nullptr);

    auto symbol_pair = current_scope->symbols.find(name);
    if (symbol_pair == current_scope->symbols.end()) {
        if (current_scope->outer_scope != nullptr) {
            symbol_pair = current_scope->outer_scope->symbols.find(name);
            if (symbol_pair == current_scope->outer_scope->symbols.end())
                return { false, std::nullopt };
        }
    }
    auto symbol = symbol_pair->second;

    if (Vector* vector = dynamic_cast<Vector*>(&symbol)) {
        if (vector->type == Vector::Type::Qbit && vector->dimension > indexed_at)
            return { true, symbol };
        else
            return { false, symbol };
    } else {
        return { false, symbol };
    }
}


std::tuple<bool, std::optional<qasm_lang::symbol_table::Symbol>>
qasm_lang::symbol_table::check_cbit_vector (std::string name, unsigned int indexed_at) {
    assert(current_scope != nullptr);

    auto symbol_pair = current_scope->symbols.find(name);
    if (symbol_pair == current_scope->symbols.end()) {
        if (current_scope->outer_scope != nullptr) {
            symbol_pair = current_scope->outer_scope->symbols.find(name);
            if (symbol_pair == current_scope->outer_scope->symbols.end())
                return { false, std::nullopt };
        }
    }
    auto symbol = symbol_pair->second;

    if (Vector* vector = dynamic_cast<Vector*>(&symbol)) {
        if (vector->type == Vector::Type::Cbit && vector->dimension > indexed_at)
            return { true, symbol };
        else
            return { false, symbol };
    } else {
        return { false, symbol };
    }
}


std::tuple<bool, std::optional<qasm_lang::symbol_table::Symbol>>
qasm_lang::symbol_table::check_gate (std::string name, unsigned int nr_parameters,
                                     unsigned int nr_arguments) {
    assert(current_scope != nullptr);

    auto symbol_pair = current_scope->symbols.find(name);
    if (symbol_pair == current_scope->symbols.end()) {
        if (current_scope->outer_scope != nullptr) {
            symbol_pair = current_scope->outer_scope->symbols.find(name);
            if (symbol_pair == current_scope->outer_scope->symbols.end())
                return { false, std::nullopt };
        }
    }
    auto symbol = symbol_pair->second;

    if (Gate *gate = dynamic_cast<Gate*>(&symbol)) {
        if (gate->nr_parameters == nr_parameters && gate->nr_arguments == nr_arguments)
            return { true, symbol };
        else
            return { false, symbol };
    } else {
        return { false, symbol };
    }
}


std::tuple<bool, std::optional<qasm_lang::symbol_table::Symbol>>
qasm_lang::symbol_table::check_gate_param (std::string name) {
    assert(current_scope != nullptr);

    auto symbol_pair = current_scope->symbols.find(name);
    if (symbol_pair == current_scope->symbols.end()) {
        if (current_scope->outer_scope != nullptr) {
            symbol_pair = current_scope->outer_scope->symbols.find(name);
            if (symbol_pair == current_scope->outer_scope->symbols.end())
                return { false, std::nullopt };
        }
    }
    auto symbol = symbol_pair->second;

    if (GateParameter* param = dynamic_cast<GateParameter*>(&symbol)) {
        return { true, symbol };
    } else {
        return { false, symbol };
    }
}
