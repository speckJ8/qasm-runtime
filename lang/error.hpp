#ifndef __QASM_LANG__ERROR_H__
#define __QASM_LANG__ERROR_H__

#include "context.hpp"
#include "input.hpp"

#include <ostream>
#include <iomanip>

namespace lang {

class Error: public std::exception {
private:
    std::string _what;

public:
    Context context;
    std::string msg;

    Error(Context ctx, std::string msg): context(ctx), msg(msg) {
        _what = "[" + context.filename + ":" +
            std::to_string(context.start_line) + ":" +
            std::to_string(context.start_col) + "] " + msg;
    }

    const char* what() const throw() {
        return _what.c_str();
    }

    void show(std::ostream& os, Input& input) {
        if (context.filename != "") {
            os << "\u001b[31m" << "===== " << context.filename << "\u001b[0m";
        }
        os << "\u001b[31m\u001b[1m" << "===== Error: " << "\u001b[0m";
        os << "\u001b[1m"  << msg << "\u001b[0m\n";
        os << "      |\n";
        int num_lines = context.start_line - context.end_line;
        for (int i = 0; i <= num_lines; i++) {
            os << std::setw(5) << context.start_line + i << " |      ";
            os << input.get_read_line(context.start_line + i) << "\n";
        }
        auto spaces = std::string((context.start_col + 5), ' ');
        auto error_marker = std::string(context.end_col - context.start_col, '~');
        os << "      |" << spaces << "\u001b[1m" << error_marker << "\u001b[0m" << "\n";
        os << "      |\n";
        // return os;
    }
};
}

#endif //  __QASM_LANG__ERROR_H__
