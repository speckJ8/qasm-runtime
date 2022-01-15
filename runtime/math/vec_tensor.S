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

    .globl vec_tensor__avx
    .type  vec_tensor__avx,@function
    .text

# void vec_tensor__avx(void* vec_a, size_t size_a, void* vec_b, size_t size_b, void* res)
#
# Compute the tensor product between complex vectors `vec_a` and `vec_b`. The size of `vec_b`
# is assumed to a multiple of 4.
vec_tensor__avx:
    push            %r12
    mov             $0, %r10
_aloop:
    mov             $0, %r11
    mov             %rcx, %r12
    imul            %r10, %r12
    vbroadcastsd    (%rdi,%r10,8), %ymm0
_bloop:
    add             %r11, %r12
    vmovsldup       (%rdx,%r11,8), %ymm1
    vmovshdup       (%rdx,%r11,8), %ymm2
    vmulps          %ymm0, %ymm1, %ymm1
    vmulps          %ymm0, %ymm2, %ymm2
    vshufps         $0b10110001, %ymm2, %ymm2, %ymm2
    vaddsubps       %ymm2, %ymm1, %ymm2
    vmovaps         %ymm2, (%r8,%r12,8)

    add             $4, %r11
    cmp             %r11, %rcx
    ja              _bloop
    add             $1, %r10
    cmp             %r10, %rsi
    ja              _aloop

    pop             %r12
    ret
