#include <stdint.h>
#include <stddef.h>

#include "periph.h"
#include "elf32.h"
#include "ihex.h"
#include "util.h"

#define CALLTABLE ((void **) 0x00200000)
#define MAXELFSIZE 0x00100000
#define MAXPROCN 128
#define MEMSTART ((unsigned char *) 0x00800000)
#define MEMEND ((unsigned char *) 0x3F000000)

unsigned char elf[MAXELFSIZE];
struct procinfo pi[MAXPROCN]; 
unsigned char *membreak = MEMSTART;
unsigned char *heapstart = ((unsigned char *) MEMEND);
struct freeblock *freehead;

struct freeblock {
	size_t size;
	struct freeblock *p;
	struct freeblock *n;
};

void heapinit()
{
	heapstart -= sizeof(struct freeblock);

	freehead = (struct freeblock *) heapstart;

	freehead->size = sizeof(struct freeblock);
	freehead->n = NULL;
	freehead->p = NULL;
}

void *malloc(size_t size)
{
	struct freeblock *p;
	
	size += sizeof(struct freeblock);

	p = freehead;
	while (p != NULL) {
		if (p->size >= size) {
			p->p->n = p->n;
			p->n->p = p->p;

			return (heapstart + sizeof(size_t));
		}

		p = p->n;
	}

	if (size > heapstart - membreak)
		return NULL;

	if (heapstart - size < membreak)
		return NULL;

	heapstart -= size;

	*((size_t *) heapstart) = size;

	return (heapstart + sizeof(size_t));
}

void free(void *p)
{
	struct freeblock *pp;

	pp = (struct freeblock *) p;

	pp->p = NULL;
	pp->n = freehead;
	
	freehead->p = pp;

	freehead = pp;
}

int notmain(void)
{
	heapinit();

	CALLTABLE[0x0000] = uart_init;
	CALLTABLE[0x0001] = uart_lcr;
	CALLTABLE[0x0002] = uart_flush;
	CALLTABLE[0x0003] = uart_send;
	CALLTABLE[0x0004] = uart_recv;
	CALLTABLE[0x0005] = uart_check;
	CALLTABLE[0x0006] = uart_overrun;
	CALLTABLE[0x0007] = timer_init;
	CALLTABLE[0x0008] = timer_tick;
	CALLTABLE[0x0009] = leds_off;
	CALLTABLE[0x000a] = malloc;

	CALLTABLE[0x1000] = util_sendstr;
	CALLTABLE[0x1001] = util_uint2hexstr;

	while (1) {
		int i;

		ih_getfile(elf);

		Elf32_load(elf, MEMSTART, MEMEND, pi);

		heapstart = ((unsigned char *) MEMEND);

		membreak += pi->imgsz;

		(pi->entry)();

		membreak -= pi->imgsz;

		for (i = 0; i < 1000; ++i)
			timer_tick();
	}

	return 0;
}
