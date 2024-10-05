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

	mov r0,pc
	bl loadkernel

hang: b hang

.globl dataflt
dataflt:
;	mov sp,#0x10000
	
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,sp,lr}

	ldr r4,kerneloff
	ldr r3,=datafault
	add r4,r4,r3
		
	;@ fault status
	mrc p15,0,r0,c5,c0,0

	;@ fault address
	mrc p15,0,r1,c6,c0,0

	blx r4

	pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,sp,lr}
	b hang
;@	bx lr

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

.globl kerneloff
kerneloff:
	.word 0x00000000
