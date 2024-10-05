#include <stdint.h>
#include <stddef.h>

#include "kernel.h"
#include "start.h"
#include "util.h"

extern uint32_t kerneloff;

static int loader_flvl(uint32_t va, uint32_t pa, unsigned int flags)
{
	uint32_t *rc;

	rc = (uint32_t *) (PAGETABLES | (va >> 18 & 0xfffffffc));

	*rc = (pa & 0xfff00000) | 0x02 | (flags & 0x7ffc);

	return 0;
}

static int loader_maprange(uint32_t va0, uint32_t va1,
	uint32_t pa0, uint32_t flags)
{
	uint32_t va;

	for (va = va0; va < va1; va += SECTIONSIZE, pa0 += SECTIONSIZE)
		loader_flvl(va, pa0, flags);

	return 0;
}

int loadkernel(void)
{
	*((volatile uint32_t *) 0x01000000) = 0x12345678;
	*((volatile uint32_t *) 0x02000000) = 0xabcdefab;
	*((volatile uint32_t *) 0x03000000) = 0xbafedcba;

	kerneloff = KERNEL - KERNELINIT;

	loader_maprange(LOADER, LOADER + LOADERSIZE, LOADER, 0x0);
	loader_maprange(KERNEL, KERNEL + KERNELSIZE, KERNELINIT, 0x0);
	loader_maprange(PAGETABLES, CALLTABLE, PAGETABLES, 0x0);

	mmuenable(PAGETABLES);
	
	(kernel + kerneloff)();
	
	return 0;
}
