.text
.globl contextTask
.type contextTask,@function
.align 16

contextTask:
    movq 0x8(%rbp), %rax
    ret
