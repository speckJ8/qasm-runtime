add_library(Unitary unitary.cc)
add_library(Vector vector.cc)
target_link_libraries(Unitary PUBLIC Vector)

target_include_directories(Unitary PUBLIC ${PROJECT_BINARY_DIR})
target_include_directories(Vector PUBLIC ${PROJECT_BINARY_DIR})

if(USE_SIMD)
    add_library(MatApplySimd mat_apply.S)
    add_library(MatMulSimd mat_mul.S)
    target_link_libraries(Unitary PRIVATE MatApplySimd MatMulSimd)
endif()

if(USE_CUDA)
endif()

add_library(Math INTERFACE)
target_link_libraries(Math INTERFACE Unitary Vector)
