#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include "third_party/cxxopts.hpp"

/**
 * Manage configurations and options set
 * through config files or command line parameters
 * */
namespace config {

/**
 * Available configuration options
 * */
enum Option {
    /**
     * Open in repl mode insted of compiling from a file
     * Command line flags: -r or --repl
     * */
    Repl,
    /**
     * Output file name
     * Command line flags: -o or --output
     * */
    Output,
    /**
     * Check wheter the program compiles without producing an output
     * Command line flags: -c or --check
     * */
    Check,
};

bool isset (Option option);
std::string get (Option option);
void parse_cmd_config (int argc, char** argv);

namespace {
    bool _repl = false;
    std::string _output = "";
    bool _check = false;
}

};

#endif // __CONFIG_H__
