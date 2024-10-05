.globl _start
_start:
	ldr pc,reseth
	ldr pc,undefh
	ldr pc,swih
	ldr pc,prefh
	ldr pc,datah
	ldr pc,hyph
	ldr pc,irqh
	ldr pc,fiqh

reseth:		.word reset
undefh:		.word dataflt
swih:		.word dataflt
prefh:		.word dataflt
datah:		.word dataflt
hyph:		.word dataflt
irqh:		.word dataflt
fiqh:		.word dataflt

reset:
	mrs r0,cpsr
	bic r0,r0,#0x1F

	orr r0,r0,#0xd3
	msr spsr_cxsf,r0
	add r0,pc,#4
	msr ELR_hyp,r0
	eret

	mrc p15, 0, r1, c12, c0, 0 ;@ get vbar
	mov r0,#0x8000
	ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
	stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}
	ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
	stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}

	mov sp,#0x10000

	bl uart_init

	mov r0,pc
	bl loadkernel

hang: b hang

.globl dataflt
dataflt:
	mov sp,#0x10000
	
	push {lr}

	ldr r1,=datafltmsg
.loop2:
	ldrb r0,[r1]

	cmp r0,#0x0
	beq .loop2out

	push {r1}
	mov r4,#0x3c000000
	sub r4, r4, #0x100000
	ldr r3,=uart_send
	add r4,r4,r3

	blx r4

;@	bl uart_send
	pop {r1}
	
	add r1,r1,#0x1

	b .loop2

.loop2out:

	mrc p15,0,r0,c6,c0,0

	mov r4,#0x3c000000
	sub r4, r4, #0x100000
	ldr r3,=sendhexint
	add r4,r4,r3

	blx r4

;@	bl sendhexint

	pop {lr}
	b hang

.globl mmuenable
mmuenable:
	mrc p15,0,r2,c1,c0,0
	bic r2,r2,#0x1
	bic r2,r2,#0x4
	bic r2,r2,#0x1000
	mcr p15,0,r2,c1,c0,0
	isb

	;@ invalidate caches
	mov r2,#0
	isb
	mcr p15,0,r2,c7,c5,0
	isb
	mcr p15,0,r2,c7,c5,6
	isb
	mcr p15,0,r2,c8,c7,0
	dsb

	;@ table base
	mov r2,#0
	mcr p15,0,r2,c2,c0,2
	isb
	mcr p15,0,r0,c2,c0,0
	isb

	;@ domain configuration
	mov r2,#0xffffffff
	mcr p15,0,r2,c3,c0,0
	isb

	;@ configuring mmu
	mrc p15,0,r2,c1,c0,0
	and r2,r2,#0xfffffffd
	orr r2,r2,#0x4
	orr r2,r2,#0x1000
	orr r2,r2,#0x1
	mcr p15,0,r2,c1,c0,0

	dsb
	
	mrc p15,0,r0,c1,c0,0

	bx lr

.globl invalidatetlbs
invalidatetlbs:
	mov r2,#0
	
	mcr p15,0,r2,c7,c5,1
	mcr p15,0,r2,c7,c6,1

	mcr p15,0,r2,c8,c7,0
	mcr p15,0,r2,c7,c10,1

	dsb

	bx lr

.globl mmudisable
mmudisable:
	mrc p15,0,r2,c1,c0,0
	bic r2,#0x4
	bic r2,#0x1
	bic r2,#0x1000
	mcr p15,0,r2,c1,c0,0

	bx lr

.globl datafltmsg
datafltmsg:
	.ascii "Data fault!!!\r\n\0"
