add_executable(MathTest math.cc)
target_link_libraries(MathTest gtest_main Math)
target_include_directories(MathTest PUBLIC "${CMAKE_SOURCE_DIR}")

gtest_discover_tests(MathTest)
