add_library(Unitary unitary.cc)
add_library(Vector vector.cc)
target_link_libraries(Unitary PUBLIC Vector)

if(USE_SIMD)
    add_library(MatApplySimd mat_apply.S)
    add_library(MatMulSimd mat_mul.S)
    add_compile_definitions(USE_SIMD)
    target_link_libraries(Unitary PRIVATE MatApplySimd MatMulSimd)
endif()

if(USE_CUDA)
endif()

add_library(Math INTERFACE)
target_link_libraries(Math INTERFACE Unitary Vector)
