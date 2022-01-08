    .globl mat_apply__avx
    .type  mat_apply__avx,@function
    .text

# void _mat_apply__avx(void* mat, void* vec, void *res, int dim)
#
# Apply the complex matrix `mat` to the complex vector `vec` and return the result in `res`.
mat_apply__avx:
    pushq        %rbx
    mov          %rcx, %rbx
_rows_loop:
    # find the number of AVX iterations necessary to compute the dot product a row
    # of `mat` and `vec`. this is done by dividing the size of the vector by 4
    mov          %rcx, %rax
    shrq         $2, %rax

    mov          %rsi, %r10
    vxorps       %ymm3, %ymm3, %ymm3
_simd_loop:
    # load duplicate values of the real components of the vector
    vmovsldup    (%rdi), %ymm0
    # load from the current row of the matrix
    vmovaps      (%r10), %ymm1
    # load duplicate values of the imaginary components of the vector
    vmovshdup    (%rdi), %ymm2
    vmulps       %ymm1, %ymm0, %ymm0
    # flip the placement of the real and imaginary components of the row
    # transforming { Re(0), Im(0), Re(1), Im(1), Re(2), Im(2), Re(3), Im(3) } into
    # { Im(0), Re(0), Im(1), Re(1), Im(2), Re(2), Im(3), Re(3) }
    vshufps      $0b10110001, %ymm1, %ymm1, %ymm1
    vmulps       %ymm1, %ymm2, %ymm2
    vaddsubps    %ymm2, %ymm0, %ymm0
    vaddps       %ymm0, %ymm3, %ymm3

    add          $32, %rdi
    add          $32, %r10
    dec          %rax
    jnz          _simd_loop

    # transform { Re(0), Im(0), Re(1), Im(1), Re(2), Im(2), Re(3), Im(3) }
    # into { Re(0), Re(1), Im(0), Im(1), Re(2), Re(3), Im(2), Im(3) }
    vshufps      $0b11011000, %ymm3, %ymm3, %ymm3
    # horizontal add produces
    # {
    #       Re(0) + Re(1), Im(0) + Im(1), Re(0) + Re(1), Im(0) + Im(1),
    #       Re(2) + Re(3), Im(2) + Im(3), Re(2) + Re(3), Im(2) + Im(3),
    # }
    vhaddps      %ymm3, %ymm3, %ymm3
    # move the high 128 bits from ymm3 to xmm4, i.e., set xmm4 to
    # { Re(2) + Re(3), Im(2) + Im(3), Re(2) + Re(3), Im(2) + Im(3) }
    vextractf128 $1, %ymm3, %xmm4
    vaddps       %xmm3, %xmm4, %xmm4
    movsd        %xmm4, (%rdx)
    add          $8, %rdx
    xorps        %xmm4, %xmm4
    dec          %rbx
    jnz          _rows_loop

    popq         %rbx

    ret
