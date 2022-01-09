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

#include "symbol_table.hpp"

#include <cassert>
#include <iostream>

namespace lang {
namespace symbol_table {

std::shared_ptr<Scope> current_scope;
std::unordered_map<std::string, std::shared_ptr<Scope>> saved_scopes;

void push_new_scope() {
    if (current_scope == nullptr) {
        current_scope = std::make_shared<Scope>();
    } else {
        current_scope = std::make_shared<Scope>(current_scope);
    }
}

void pop_scope() {
    if (current_scope == nullptr)
        push_new_scope();

    std::shared_ptr<Scope> outer_scope = current_scope->outer_scope;
    current_scope.reset();
    current_scope = outer_scope;
}

void pop_and_save_scope(std::string name) {
    std::shared_ptr<Scope> outer_scope = current_scope->outer_scope;
    std::shared_ptr<Scope> old_scope = current_scope;
    current_scope = outer_scope;

    // save the scope if it is not already saved
    if (saved_scopes.find(name) == saved_scopes.end()) {
        saved_scopes.insert(std::make_pair(name, old_scope));
    }
}

bool restore_scope(std::string name) {
    auto _scope = saved_scopes.find(name);
    if (_scope == saved_scopes.end()) {
        return false;
    } else {
        auto scope = _scope->second;
        scope->outer_scope = current_scope;
        current_scope = scope;
        return true;
    }
}

void declare(std::shared_ptr<symbol_table::Symbol> s) {
    if (current_scope == nullptr)
        push_new_scope();
    current_scope->symbols.insert({ s->name, s });
}

std::optional<std::shared_ptr<Symbol>> get(std::string name, bool this_scope) {
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

void dump() {
    assert(current_scope != nullptr);

    std::cout << "--------------[ root ]--------------\n";
    for (auto& element : current_scope->symbols) {
        auto symbol = element.second;
        std::cout << symbol->to_string() << "\n";
    }
    for (auto& [ name, scope ] : saved_scopes) {
        std::cout << "\n--------------[ scope: " << name << " ]--------------\n";
        assert(scope != nullptr);
        for (auto& element : scope->symbols) {
            auto symbol = element.second;
            std::cout << symbol->to_string() << "\n";
        }
    }
}

}
}
