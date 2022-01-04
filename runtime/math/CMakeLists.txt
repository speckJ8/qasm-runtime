add_library(Math unitary.cc vector.cc)

if(USE_SIMD)
    add_library(MathSimd simd.cc simd.S)
    list(APPEND EXTRA_MATH_LIBS MathSimd)
    target_compile_definitions(Math PRIVATE USE_SIMD)
endif()

target_link_libraries(Math PRIVATE ${EXTRA_MATH_LIBS})
