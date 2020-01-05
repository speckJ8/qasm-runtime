#include "config.h"
#include <iostream>
#include <cstdlib>

void config::parse_cmd_config (int argc, char** argv) {
    try {
        cxxopts::Options options(argv[0], "OpenQASM Simulator");
        options.add_options()
            ("r,repl", "open in repl mode", cxxopts::value<bool>(_repl))
            ("c,check", "just check if program compiles, without generating output",
             cxxopts::value<bool>(_repl))
            ("o,output", "name of output file", cxxopts::value<bool>(_repl));
        options.parse(argc, argv);
    } catch (const cxxopts::OptionException& e) {
        std::cout << "Failed to parse command line arguments: " << e.what() << "\n";
        exit(1);
    }
}
