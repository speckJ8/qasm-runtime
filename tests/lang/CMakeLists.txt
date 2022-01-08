add_executable(LexerTest lexer.cc)
target_link_libraries(LexerTest gtest_main Lexer)
target_include_directories(LexerTest PUBLIC "${CMAKE_SOURCE_DIR}")

gtest_discover_tests(LexerTest)
