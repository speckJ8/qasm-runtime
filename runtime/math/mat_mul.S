# Copyright (c) 2022 Alcides Andrade <andrade.alcides.junior@gmail.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

    .globl mat_mul__avx
    .type  mat_mul__avx,@function
    .text


# void mat_mul__avx(void* mat_a, void* mat_b, void *res, int dim)
#
# Multiply complex square matrices `mat_a` and `mat_b` and return the result in `res`.
# The dimension of the matrices (size of row and columns) is assumed to be nonzero and
# a multiple of 4.
mat_mul__avx:
    push            %r12
    mov             $0, %r8
_rows_loop:
    mov             %rcx, %r11
    imul            %r8, %r11
    mov             $0, %r9
_cols_loop:
    mov             %r11, %rax
    add             %r9, %rax
    vbroadcastsd    (%rdi,%rax,8), %ymm0
    mov             %rcx, %r12
    imul            %r9, %r12
    mov             $0, %r10
_simd_loop:
    mov             %r12, %rax
    add             %r10, %rax
    vmovsldup       (%rsi,%rax,8), %ymm1
    vmovshdup       (%rsi,%rax,8), %ymm2
    vmulps          %ymm0, %ymm1, %ymm1
    vmulps          %ymm0, %ymm2, %ymm2
    vshufps         $0b10110001, %ymm2, %ymm2, %ymm2
    vaddsubps       %ymm2, %ymm1, %ymm2
    mov             %r11, %rax
    add             %r10, %rax
    vaddps          (%rdx,%rax,8), %ymm2, %ymm3
    vmovaps         %ymm3, (%rdx,%rax,8)

    add             $4, %r10
    cmp             %r10, %rcx
    ja              _simd_loop
    add             $1, %r9
    cmp             %r9, %rcx
    ja              _cols_loop
    add             $1, %r8
    cmp             %r8, %rcx
    ja              _rows_loop

    pop             %r12
    ret
