#include "input.hpp"

#include <cassert>
#include <fstream>
#include <ios>
#include <sstream>
#include <iostream>

namespace lang {

char Input::peek() {
    return this->_stream.peek();
}

char Input::get() {
    char next = this->_stream.get();
    if (next == '\n') {
        this->line++;
        this->col = 1;
        this->_lines_read.push_back(std::move(this->_curr_line));
        this->_curr_line = "";
    } else {
        this->_curr_line.push_back(next);
        this->col++;
    }
    return next;
}

bool Input::eof() {
    return this->_stream.eof();
}

void Input::strip_spaces() {
    while (std::isspace(this->_stream.peek()) && !this->_stream.eof()) {
        char next = this->_stream.get();
        if (next == '\n' || next == '\r') {
            this->line++;
            this->col = 1;
            this->_lines_read.push_back(std::move(this->_curr_line));
            this->_curr_line = "";
        } else {
            this->col++;
            this->_curr_line.push_back(next);
        }
    }
}

Context Input::context(unsigned int start_line, unsigned int start_col) {
    return Context(start_line, this->line, start_col, this->col, this->filename);
}

std::string Input::get_read_line(unsigned int line) {
    assert(line < this->_lines_read.size());
    return this->_lines_read[line-1];
}

}
