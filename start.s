.globl _start
_start:
	mrs r0,cpsr
	bic r0,r0,#0x1F

	orr r0,r0,#0xd3
	msr spsr_cxsf,r0
	add r0,pc,#4
	msr ELR_hyp,r0
	eret

	mov sp,#0x8000
	mov r0,pc
	bl notmain

hang: b hang

.globl PUTW
PUTW:
	str r1,[r0]
	bx lr

.globl GETW
GETW:
	ldr r0,[r0]
	bx lr

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

.globl dummy
dummy:
	bx lr


;@-------------------------------------------------------------------------
;@
;@ Copyright (c) 2012 David Welch dwelch@dwelch.com
;@
;@ Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
;@
;@ The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
;@
;@ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
;@
;@-------------------------------------------------------------------------
