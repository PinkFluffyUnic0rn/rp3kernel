#include <stdint.h>
#include <stddef.h>

#include "periph.h"
#include "elf32.h"
#include "ihex.h"
#include "util.h"

#define CALLTABLE 0x3e000000
#define MAXELFSIZE 0x00100000
#define MAXPROCN 128
#define MEMSTART ((unsigned char *) 0x00008000)
#define MEMEND ((unsigned char *) 0x10000000)
#define ADDREND 0x40000000

#define PAGETABLES 0x3d000000
#define SECTIONSIZE 0x100000

// 0x0
// 0x00008000		start
//
//
// 0x3d000000		PAGETABLES
// 0x3e000000		CALLTABLE
// 0x3f000000		PBASE
// 0x40000000		ADDREND

unsigned char elf[MAXELFSIZE];
struct procinfo pi[MAXPROCN]; 
unsigned char *membreak = MEMSTART;

unsigned char *heapstart = ((unsigned char *) MEMEND);
struct block *freehead;

struct block {
	size_t size;
	struct block *p;
	struct block *n;
};

int mmuenable(uint32_t);
int mmudisable();
int invalidatetlbs();

void heapinit()
{
	heapstart = MEMEND - sizeof(struct block);

	freehead = (struct block *) heapstart;

	freehead->size = sizeof(struct block);
	freehead->n = NULL;
	freehead->p = NULL;
}

void *malloc(size_t size)
{
	struct block *p;

	size += sizeof(struct block);
	size = (size & 0xfffffff8) + 0x8;

	p = freehead;
	while (p != NULL) {
		if (p->size >= size) {
			if (p->p != NULL)
				p->p->n = p->n;
			else
				freehead = p->n;

			if (p->n != NULL)
				p->n->p = p->p;

			return ((unsigned char *) p + sizeof(struct block));
		}

		p = p->n;
	}

	if (heapstart - size < membreak)
		return NULL;

	heapstart -= size;
	
	*((size_t *) heapstart) = size;

	return (heapstart + sizeof(struct block));
}

void free(void *ptr)
{
	struct block *fb;

	ptr -= sizeof(struct block);

	fb = (struct block *) ptr;

	fb->p = NULL;
	fb->n = freehead;
	
	freehead->p = fb;

	freehead = fb;

	for (fb = freehead; fb != NULL;) {
		size_t sz;
	
		sz = *((size_t *) fb);

		if (((unsigned char * ) fb) + sz == heapstart) {
			if (fb->p != NULL)
				fb->p->n = fb->n;
			else
				freehead = fb->n;
			
			if (fb->n != NULL)
				fb->n->p = fb->p;

			fb = fb->n;
		
			heapstart += sz;
		
			continue;
		}
 
		fb = fb->n;
	}
}

int mmu_flvl(uint32_t va, uint32_t pa, unsigned int flags)
{
	uint32_t *rc;

	rc = (uint32_t *) (PAGETABLES | (va >> 18 & 0xfffffffc));

	*rc = (pa & 0xfff00000) | 0x02 | (flags & 0x7ffc);

	return 0;
}

int mmu_maprange(uint32_t va0, uint32_t va1, uint32_t pa0, uint32_t flags)
{
	uint32_t va;

	for (va = va0; va < va1; va += SECTIONSIZE, pa0 += SECTIONSIZE)
		mmu_flvl(va, pa0, flags);

	return 0;
}

int initcalltable()
{
	void **ct;

	ct = (void **) CALLTABLE;

	ct[0x0000] = uart_init;
	ct[0x0001] = uart_lcr;
	ct[0x0002] = uart_flush;
	ct[0x0003] = uart_send;
	ct[0x0004] = uart_recv;
	ct[0x0005] = uart_check;
	ct[0x0006] = uart_overrun;
	ct[0x0007] = timer_init;
	ct[0x0008] = timer_tick;
	ct[0x0009] = leds_off;
	ct[0x000a] = malloc;
	ct[0x000b] = free;

	ct[0x1000] = util_sendstr;
	ct[0x1001] = util_uint2hexstr;
	ct[0x1002] = util_uint2str;
	ct[0x1003] = util_sendhexint;
	ct[0x1004] = util_senduint;
	ct[0x1005] = util_sendint;

	return 0;
}

int notmain(void)
{
	volatile uint32_t *addrtest;
	volatile uint32_t *addrtest2;
	volatile uint32_t *addrtest3;
	uint32_t r;

//	heapinit();
	
	uart_init();
	
	util_sendstr("\r\n\r\n\r\nhello!\r\n");

	addrtest = (volatile uint32_t *) 0x00100000;
	addrtest2 = (volatile uint32_t *) 0x00200000;
	addrtest3 = (volatile uint32_t *) 0x00300000;

	*addrtest = 0x12345678;
	*addrtest2 = 0xabcdefab;
	*addrtest3 = 0xbafedcba;

	mmu_maprange(0x0, MAXELFSIZE, 0x0, 0x0);
	mmu_maprange(PAGETABLES, CALLTABLE, PAGETABLES, 0x0);
	mmu_maprange(PBASE, ADDREND, PBASE, 0x0);
	mmu_maprange(0x00100000, 0x00200000, 0x00300000, 0x0);

	util_printf("%x value before: %x\r\n", addrtest2, *addrtest2);
	util_printf("%x value before: %x\r\n", addrtest, *addrtest);

	r = mmuenable(PAGETABLES);
    	
	util_printf("enable result: %x\r\n", r);
	util_printf("%x value before: %x\r\n", addrtest, *addrtest);
	
	util_sendstr("after test!\r\n");

	while (1) {
		int i;

	//	util_sendstr("hello!\r\n");

/*
		ih_getfile(elf);

		Elf32_load(elf, MEMSTART, MEMEND, pi);

		membreak += pi->imgsz;

		(pi->entry)();

		membreak -= pi->imgsz;
*/
		for (i = 0; i < 1000; ++i)
			timer_tick();
	}

	return 0;
}
