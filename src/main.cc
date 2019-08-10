#include <sstream>
#include "qasm_lang/parser.h"

int main (int argc, char** argv) {
    std::stringstream input("the string");
    qasm_lang::parser::parse(input);
    return 0;
}
