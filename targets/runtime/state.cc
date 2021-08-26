#include "state.h"

runtime::State::State (std::vector<runtime::Register> registers) {
    for (auto& r : registers) {
        std::cout << "register " << r.name << "[" << r.size << "]\n";
    }
}

void runtime::State::apply (runtime::Qgate gate, runtime::Register register_) {
}
