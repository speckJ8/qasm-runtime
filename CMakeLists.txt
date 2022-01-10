cmake_minimum_required(VERSION 3.10)

project(QasmRuntime)
enable_language(CUDA ASM)
enable_testing()

add_compile_options(
    -Wall
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:RELEASE>:-O3>
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(lang)
add_subdirectory(runtime)
add_subdirectory(tests)

add_executable(Qasm qasm.cc)
target_link_libraries(Qasm PUBLIC Lang)
target_include_directories(Qasm PUBLIC "${PROJECT_BINARY_DIRECTORY}")

configure_file(config.h.in config.h)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
