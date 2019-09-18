.text
.globl contextSwitch
.type contextSwitch,@function
.align 16

/**
 * Arguments:
 * RDI: 1st argument
 * RSI: 2nd argument
 *
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
	stmxcsr (%rsp) /* save MMX control- and status-word */
    fnstcw 0x4(%rsp) /* save x87 control-word */

    movq %r12, 0x8(%rsp)  /* save R12 */
    movq %r13, 0x10(%rsp)  /* save R13 */
    movq %r14, 0x18(%rsp)  /* save R14 */
    movq %r15, 0x20(%rsp)  /* save R15 */
    movq %rbx, 0x28(%rsp)  /* save RBX */
    movq %rbp, 0x30(%rsp)  /* save RBP */

	// Save current stack pointer
	movq %rsp, (%rdi)

	// Move the stack to the new context
	movq %rsi, %rsp

	// Restore registers
    ldmxcsr (%rsp) /* restore MMX control- and status-word */
    fldcw 0x4(%rsp) /* restore x87 control-word */

    movq 0x8(%rsp), %r12 /* restore R12 */
    movq 0x10(%rsp), %r13 /* restore R13 */
    movq 0x18(%rsp), %r14 /* restore R14 */
    movq 0x20(%rsp), %r15 /* restore R15 */
    movq 0x28(%rsp), %rbx /* restore RBX */
    movq 0x30(%rsp), %rbp /* restore RBP */

	// Return address
	movq 0x38(%rsp), %r8

    leaq 0x40(%rsp), %rsp /* prepare stack */

	jmp *%r8
