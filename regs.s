.globl PUTW
PUTW:
	str r1,[r0]
	bx lr

.globl GETW
GETW:
	ldr r0,[r0]
	bx lr

.globl getpc
getpc:
	mov r0,pc

	bx lr

.globl dummy
dummy:
	bx lr
