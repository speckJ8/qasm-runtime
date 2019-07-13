#include "src/qasm_lang/lexer.h"
#include "gtest/gtest.h"

#include <sstream>
#include <iostream>


namespace {
class LexerTest : public ::testing::Test {};
}

const char* ok_sample =
"OPENQASM 2.0;\
include \"file.qasm\";\
qreg q[3];\
qreg a[2];\
creg c[3];\
creg syn[2];\
gate syndrome d1,d2,d3,a1,a2\
{\
cx d1,a1; cx d2,a1;\
cx d2,a2; cx d3,a2;\
}\
x q[0];\
barrier q;\
syndrome q[0],q[1],q[2],a[0],a[1];\
measure a -> syn;\
if(syn==1) x q[0];\
if(syn==2) x q[2];\
if(syn==3) x q[1];\
measure q -> c;";


TEST_F(LexerTest, SimpleTest) {
    using namespace qasm_lang::lexer;
 
    std::stringstream sample((std::string(ok_sample)));
    unsigned int curr_line = 0;
    while (true) {
        Token tk = lex(sample, curr_line);
        curr_line = tk.line;
        if (tk.type == Token::Eof) break;
        std::cout << "Found token: " << tk.value << " of type: " << tk.type << std::endl;
        ASSERT_EQ(tk.type != Token::Invalid, true);
    }
}



int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
