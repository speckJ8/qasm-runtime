cmake_minimum_required(VERSION 3.10)

project(QasmRuntime)
enable_language(CUDA ASM)

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
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    enable_testing()
    add_subdirectory(tests)
endif()

add_executable(Qasm qasm.cc)

target_link_libraries(Qasm PUBLIC Error)
target_link_libraries(Qasm PUBLIC Input)
target_link_libraries(Qasm PUBLIC Parser)
target_link_libraries(Qasm PUBLIC Program)
target_link_libraries(Qasm PUBLIC Sema)
target_link_libraries(Qasm PUBLIC SymbolTable)
target_link_libraries(Qasm PUBLIC Runtime)

target_include_directories(Qasm PUBLIC "${PROJECT_BINARY_DIRECTORY}")

option(USE_SIMD "Use SIMD for math operations" ON)
option(USE_CUDA "Use Cuda for math operations" OFF)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
