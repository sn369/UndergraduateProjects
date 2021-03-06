.globl	api_putstrwin
.include "syscall.def"

# void api_putstrwin(int win, int x, int y, int col, int len, const char* str)
api_putstrwin:
	push	%edi
	push	%esi
	push	%ebp
	push	%ebx
	mov	20(%esp), %ebx	# win
	mov	24(%esp), %esi	# x
	mov	28(%esp), %edi	# y
	mov	32(%esp), %eax	# col
	mov	36(%esp), %ecx	# len
	mov	40(%esp), %ebp	# str
	syscall	API_PUTSTRWIN
	pop	%ebx
	pop	%ebp
	pop	%esi
	pop	%edi
	ret
