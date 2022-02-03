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

#define ID(name) _current_scope->_id + "::" + name

namespace lang {
namespace symbol_table {

std::shared_ptr<Scope> _current_scope;
std::unordered_map<std::string, std::shared_ptr<Scope>> _saved_scopes;

void push_scope(std::string name) {
    if (_current_scope == nullptr) {
        _current_scope = std::make_shared<Scope>(name);
        _saved_scopes.insert({name, _current_scope});
    } else {
        auto id = ID(name);
        _current_scope = std::make_shared<Scope>(_current_scope, id);
        _saved_scopes.insert({id, _current_scope});
    }
}

void pop_scope() { _current_scope = _current_scope->_outer_scope; }

bool restore_scope(std::string name) {
    auto id = ID(name);
    auto scope = _saved_scopes.find(id);
    if (scope == _saved_scopes.end()) {
        return false;
    } else {
        _current_scope = scope->second;
        return true;
    }
}

void declare(std::shared_ptr<symbol_table::Symbol> s) {
    assert(_current_scope != nullptr);
    _current_scope->_symbols.insert({s->name, s});
}

std::optional<std::shared_ptr<Symbol>> get(std::string name, bool this_scope) {
    assert(_current_scope != nullptr);

    if (this_scope) {
        auto symbol = _current_scope->_symbols.find(name);
        if (symbol != _current_scope->_symbols.end()) return std::optional(symbol->second);
    } else {
        auto scope = _current_scope;
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
    for (auto& [name, scope] : _saved_scopes) {
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
