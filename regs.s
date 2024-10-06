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

.globl getvbar
getvbar:
	mrc p15,0,r0,c12,c0,0

	bx lr

.globl invalidatetlbs
invalidatetlbs:
	mov r2,#0
	mcr p15,0,r2,c7,c5,0
	mcr p15,0,r2,c7,c5,6
	mcr p15,0,r2,c8,c7,0
	dsb

	bx lr

.globl dummy
dummy:
	bx lr
