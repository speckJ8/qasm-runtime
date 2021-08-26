#include "symbol_table.h"
#include <cassert>
#include <iostream>

void qasm::symbol_table::push_new_scope () {
    if (current_scope == nullptr) {
        current_scope = std::make_shared<Scope>();
    } else {
        current_scope = std::make_shared<Scope>(std::move(current_scope));
    }
}

void qasm::symbol_table::pop_scope () {
    if (current_scope == nullptr)
        push_new_scope();

    std::shared_ptr<Scope> outer_scope = std::move(current_scope->outer_scope);
    current_scope.reset();
    current_scope = std::move(outer_scope);
}

void qasm::symbol_table::pop_and_save_scope (std::string name) {
    std::shared_ptr<Scope> outer_scope = std::move(current_scope->outer_scope);
    std::shared_ptr<Scope> old_scope = std::move(current_scope);
    current_scope = std::move(outer_scope);

    // save the scope if it is not already saved
    if (current_scope->saved_scopes.find(name) == current_scope->saved_scopes.end()) {
        current_scope->saved_scopes.insert(std::make_pair(name, std::move(old_scope)));
    }
}

bool qasm::symbol_table::restore_scope (std::string name) {
    auto _scope = current_scope->saved_scopes.find(name);
    if (_scope == current_scope->saved_scopes.end()) {
        return false;
    } else {
        auto scope = std::move(_scope->second);
        scope->outer_scope = std::move(current_scope);
        current_scope = std::move(scope);
        return true;
    }
}

void qasm::symbol_table::declare (std::shared_ptr<qasm::symbol_table::Symbol> s) {
    if (current_scope == nullptr)
        push_new_scope();
    current_scope->symbols.insert({ s->name, s });
}

std::optional<std::shared_ptr<qasm::symbol_table::Symbol>>
qasm::symbol_table::get (std::string name, bool this_scope) {
    if (current_scope == nullptr)
        push_new_scope();

    if (this_scope) {
        auto symbol = current_scope->symbols.find(name);
        if (symbol != current_scope->symbols.end())
            return std::optional(symbol->second);
    } else {
        auto scope = current_scope;
        while (scope != nullptr) {
            auto symbol = scope->symbols.find(name);
            if (symbol != scope->symbols.end()) {
                return std::optional(symbol->second);
            } else {
                scope = scope->outer_scope;
            }
        }
    }

    return std::nullopt;
}

void qasm::symbol_table::dump () {
    assert(current_scope != nullptr);

    std::cout << "------- root -----------------------------------------------\n";
    for (auto element : current_scope->symbols) {
        auto symbol = element.second;
        std::cout << symbol->to_string() << "\n";
    }
    std::cout << "-------------------------------------------------------------\n\n";
    for (auto _scope : current_scope->saved_scopes) {
        auto scope = _scope.second;
        auto name = _scope.first;
        std::cout << "------- scope: " << std::setw(15) << name
                  << " ------------------------------\n";
        for (auto element : scope->symbols) {
            auto symbol = element.second;
            std::cout << symbol->to_string() << "\n";
        }
        std::cout << "-------------------------------------------------------------\n\n";
    }
}
