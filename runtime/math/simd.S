    .global _mat_apply__sse3
    .type   _mat_apply__sse3,@function
    .text

# void _mat_apply__sse3(void* mat, void* vec, void *res, int dim, int num_simd_loops)
#
# Apply the complex matrix `mat` to the complex vector `vec` and return the result in `res`.
_mat_apply__sse3:
    mov         $2, %r11

_row_loop:
    mov         %r8, %rax
    mov         %rsi, %r10
    xorps       %xmm3, %xmm3

_simd_loop:
    movsldup    (%rdi), %xmm0
    movaps      (%r10), %xmm1
    movshdup    (%rdi), %xmm2
    mulps       %xmm1, %xmm0
    shufps      $0b10110001, %xmm1, %xmm1
    mulps       %xmm1, %xmm2
    addsubps    %xmm2, %xmm0
    shufps      $0b11011000, %xmm0, %xmm0
    haddps      %xmm0, %xmm0
    addps       %xmm0, %xmm3

    add         $16, %rdi
    add         $16, %r10
    dec         %rax
    jnz         _simd_loop

    dec         %r11
    jz          _store_result
    xorps       %xmm4, %xmm4
    shufps      $0b11100100, %xmm4, %xmm3
    movaps      %xmm3, %xmm4
    jmp         _next_row_loop_iter_setup
_store_result:
    mov         $2, %r11
    shufps      $0b01000100, %xmm3, %xmm4
    movaps      %xmm4, (%rdx)
    add         $16, %rdx
_next_row_loop_iter_setup:
    dec         %rcx
    jnz         _row_loop

    ret
