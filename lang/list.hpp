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

#ifndef __QASM_LANG__AST_LIST_H__
#define __QASM_LANG__AST_LIST_H__

#include "expression.hpp"
#include "context.hpp"

#include <vector>
#include <string>
#include <memory>

namespace lang {

class IdentifierList {
public:
    Context context;
    std::vector<std::string> id_list;

    IdentifierList(std::vector<std::string> il, Context c):
        context(c), id_list(il)
    {};
};

class MixedList {
public:
    Context context;
    std::vector<Variable> mixed_list;

    MixedList(std::vector<Variable> ml, Context c):
        context(c), mixed_list(ml)
    {};
};

class ExpressionList {
public:
    Context context;
    std::vector<std::shared_ptr<Expression>> expression_list;

    ExpressionList(std::vector<std::shared_ptr<Expression>> el, Context c):
        context(c),
        expression_list(std::move(el))
    {};
};

}

#endif // __QASM_LANG__AST_LIST_H__
