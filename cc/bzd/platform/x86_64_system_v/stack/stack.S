.text
.globl contextSwitch
.type contextSwitch,@function
.align 16

/**
 * | ---- Stack --- |
 * |    @ret addr   | RSP + 0x38
 * |       RBP      | RSP + 0x30
 * |       RBX      | RSP + 0x28
 * |       R15      | RSP + 0x20 
 * |       R14      | RSP + 0x18
 * |       R13      | RSP + 0x10
 * |       R12      | RSP + 0x8
 * |    MMX & x87   | RSP
 */
contextSwitch:

	// Reserve 0x38 bytes on the current stack
	leaq -0x38(%rsp), %rsp

	// Save registers
	stmxcsr (%rsp) // Save MMX control- and status-word
    fnstcw 0x4(%rsp) // Save x87 control-word

    movq %r12, 0x8(%rsp) // Save R12
    movq %r13, 0x10(%rsp) // Save R13
    movq %r14, 0x18(%rsp) // Save R14
    movq %r15, 0x20(%rsp) // Save R15
    movq %rbx, 0x28(%rsp) // Save RBX
    movq %rbp, 0x30(%rsp) // Save RBP

	// Save current stack pointer
	movq %rsp, (%rdi)

	// Move the stack to the new context
	movq %rsi, %rsp

	// Restore registers
    ldmxcsr (%rsp) // Restore MMX control- and status-word
    fldcw 0x4(%rsp) // Restore x87 control-word

    movq 0x8(%rsp), %r12 // Restore R12
    movq 0x10(%rsp), %r13 // Restore R13
    movq 0x18(%rsp), %r14 // Restore R14
    movq 0x20(%rsp), %r15 // Restore R15
    movq 0x28(%rsp), %rbx // Restore RBX
    movq 0x30(%rsp), %rbp // Restore RBP

	// Return address
	movq 0x38(%rsp), %r8

    leaq 0x40(%rsp), %rsp // Prepare stack

	jmp *%r8
