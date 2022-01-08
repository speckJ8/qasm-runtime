add_library(Math unitary.cc vector.cc)

if(USE_SIMD)
    add_library(MathSimd mat_apply.S)
    list(APPEND EXTRA_MATH_LIBS MathSimd)
    target_compile_definitions(Math PRIVATE USE_SIMD)
endif()

if(USE_CUDA)
    add_library(MathCuda cuda.cc)
    list(APPEND EXTRA_MATH_LIBS MathCuda)
    target_compile_definitions(Math PRIVATE USE_CUDA)
endif()

target_link_libraries(Math PRIVATE ${EXTRA_MATH_LIBS})
