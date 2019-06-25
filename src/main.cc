#include <iostream>

template<typename... Args>
auto sum (Args... args) {
    return (... + args);
}

int main (int argc, char** argv) {
    std::cout << "hello bazel";
    auto return_val { sum(1,2,3) };
    return return_val;
}
