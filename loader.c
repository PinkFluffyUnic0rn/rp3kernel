#include <stdint.h>
#include <stddef.h>

#include "kernel.h"
#include "start.h"
#include "util.h"

#define KERNELINIT 0x100000

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

void dataflt();

int loadkernel(void)
{
	*((volatile uint32_t *) 0x01000000) = 0x12345678;
	*((volatile uint32_t *) 0x02000000) = 0xabcdefab;
	*((volatile uint32_t *) 0x03000000) = 0xbafedcba;

	mmu_maprange(LOADER, LOADER + LOADERSIZE, LOADER, 0x0);
	mmu_maprange(KERNEL, KERNEL + KERNELSIZE, KERNELINIT, 0x0);
	mmu_maprange(PAGETABLES, CALLTABLE, PAGETABLES, 0x0);
	mmu_maprange(PBASE, ADDREND, PBASE, 0x0);
	mmu_maprange(0x01000000, 0x02000000, 0x03000000, 0x0);

	mmuenable(PAGETABLES);
	
	(kernel + KERNEL - KERNELINIT)();
	
	return 0;
}
