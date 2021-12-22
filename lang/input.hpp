#ifndef __QASM_LANG__INPUT_H__
#define __QASM_LANG__INPUT_H__

#include "context.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <optional>
#include <vector>


namespace lang {

class Input {
public:
    Input(std::istream& stream): _stream(stream) {};
    Input(std::istream& stream, std::string filename):
        filename(filename), _stream(stream){};

    char peek();
    char get();
    bool eof();
    void strip_spaces();
    std::string get_read_line(unsigned int line);

    Context context(unsigned int start_line, unsigned int start_col);

    unsigned int line { 1 };
    unsigned int col { 1 };
    std::string filename { "" };

private:
    std::vector<std::string> _lines_read;
    std::string _curr_line { "" };
    std::istream& _stream;
};

}

#endif // __QASM_LANG__INPUT_H__
