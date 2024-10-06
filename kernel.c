#include <stdint.h>

#include "elf32.h"
#include "periph.h"
#include "util.h"
#include "start.h"

/*
static unsigned char elf[MAXELFSIZE];
static struct procinfo pi[MAXPROCN]; 
*/

uint32_t getvbar();
void invalidatetlbs();

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

int mmu_printflvl(uint32_t va)
{
	uint32_t *rc;

	rc = (uint32_t *) (PAGETABLES | (va >> 18 & 0xfffffffc));

	printf("%x: %x\r\n", (uint32_t) rc, *rc);

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

	ct[0x1000] = sendstr;
	ct[0x1001] = uint2hexstr;
	ct[0x1002] = uint2str;
	ct[0x1003] = sendhexint;
	ct[0x1004] = senduint;
	ct[0x1005] = sendint;
	ct[0x1006] = malloc;
	ct[0x1007] = free;
	ct[0x1008] = printf;

	return 0;
}

int undefined(uint32_t lr)
{
	printf("\r\nindefined: %x\r\n", lr);

	return 0;
}

int swi()
{
	printf("\r\nswi\r\n");
	
	return 0;
}

int prefetch()
{
	printf("\r\nprefetch\r\n");
	
	return 0;
}

int datafault(uint32_t pc, uint32_t status, uint32_t addr)
{
	uint32_t pageaddr;

	printf("\r\ndata fault: %x %x %x\r\n", pc, status, addr);

	pageaddr = addr & 0xfff00000;
	
	mmu_maprange(pageaddr, pageaddr + 0x00100000, 0x02000000, 0x0);
	invalidatetlbs();

	return 0;
}

int hyp()
{
	printf("\r\nhyp\r\n");
	
	return 0;
}

int irq()
{
	printf("\r\nirq\r\n");
	
	return 0;
}

int fiq()
{
	printf("\r\nfiq\r\n");
	
	return 0;
}

int kernel()
{
	volatile uint32_t *addrtest;
	
	mmu_maprange(PBASE, ADDREND, PBASE, 0x0);
	mmu_maprange(0x01000000, 0x02000000, 0x03000000, 0x0);
	invalidatetlbs();
	
	uart_init();

	addrtest = (volatile uint32_t *) 0x01000000;
	printf("%x value: %x\r\n", addrtest, *addrtest);

	printf("\r\npage table before:\r\n");
	mmu_printflvl(LOADER);
	mmu_printflvl(KERNEL);
	mmu_printflvl(PAGETABLES);
	mmu_printflvl(PBASE);
	mmu_printflvl(0x01000000);
	mmu_printflvl(0x03000000);

	*((volatile uint32_t *) 0x03000000) = 0x11223344;
	
	printf("\r\npage table after:\r\n");
	mmu_printflvl(LOADER);
	mmu_printflvl(KERNEL);
	mmu_printflvl(PAGETABLES);
	mmu_printflvl(PBASE);
	mmu_printflvl(0x01000000);
	mmu_printflvl(0x03000000);

	printf("pc value: %x\r\n", getpc());
	
	addrtest = (volatile uint32_t *) 0x03000000;
	printf("%x value after: %x\r\n", addrtest, *addrtest);


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
}
