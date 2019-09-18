.text
.globl contextGet
.type contextGet,@function
.align 16

contextGet:
    movq 0x8(%rbp), %rax
    ret
