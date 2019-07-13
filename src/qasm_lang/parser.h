#pragma once

#include "third_party/pegtl.hpp"

namespace qasm_lang {

using namespace tao::pegtl;

namespace parser {

    struct number : plus<digit> {};

};

};
