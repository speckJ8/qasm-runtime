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

#define ID(name) current_scope->_id + "::" + name

std::shared_ptr<Scope> current_scope;
std::unordered_map<std::string, std::shared_ptr<Scope>> saved_scopes;

void push_scope(std::string name) {
    if (current_scope == nullptr) {
        current_scope = std::make_shared<Scope>(name);
        saved_scopes.insert({ name, current_scope });
    } else {
        auto id = ID(name);
        current_scope = std::make_shared<Scope>(current_scope, id);
        saved_scopes.insert({ id, current_scope });
    }
}

void pop_scope() {
    current_scope = current_scope->_outer_scope;
}

bool restore_scope(std::string name) {
    auto id = ID(name);
    auto scope = saved_scopes.find(id);
    if (scope == saved_scopes.end()) {
        return false;
    } else {
        current_scope = scope->second;
        return true;
    }
}

void declare(std::shared_ptr<symbol_table::Symbol> s) {
    assert(current_scope != nullptr);
    current_scope->_symbols.insert({ s->name, s });
}

std::optional<std::shared_ptr<Symbol>> get(std::string name, bool this_scope) {
    assert(current_scope != nullptr);

    if (this_scope) {
        auto symbol = current_scope->_symbols.find(name);
        if (symbol != current_scope->_symbols.end())
            return std::optional(symbol->second);
    } else {
        auto scope = current_scope;
        while (scope != nullptr) {
            auto symbol = scope->_symbols.find(name);
            if (symbol != scope->_symbols.end()) {
                return std::optional(symbol->second);
            } else {
                scope = scope->_outer_scope;
            }
        }
    }

    return std::nullopt;
}

void dump() {
    for (auto& [ name, scope ] : saved_scopes) {
        std::cout << "--------------[ scope: " << name << " ]--------------\n";
        assert(scope != nullptr);
        for (auto& element : scope->_symbols) {
            auto symbol = element.second;
            std::cout << symbol->to_string() << "\n";
        }
        std::cout << "\n";
    }
}

}
}
