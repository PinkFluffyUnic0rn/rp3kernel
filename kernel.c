#include <stdint.h>

#include "periph.h"
#include "elf32.h"
#include "ihex.h"
#include "util.h"

#define CALLTABLE ((void **) 0x00200000)
#define MAXELFSIZE (1024 * 1024)
#define MAXPROCN 128
#define MEMSTART 0x00800000
#define MEMEND 0x40000000

char buf[1024];

unsigned char elf[MAXELFSIZE];
struct procinfo pi[MAXPROCN]; 
size_t membreak = MEMSTART;

void BRANCHTO(unsigned int);

int notmain(void)
{
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

	CALLTABLE[0x1000] = util_sendstr;
	CALLTABLE[0x1001] = util_uint2hexstr;

	while (1) {
		int i;

		ih_getfile(elf);

		Elf32_load(elf, (unsigned char *) MEMSTART, MEMEND, pi);

		BRANCHTO((unsigned int) pi->entry);

		for (i = 0; i < 1000; ++i)
			timer_tick();
	}

	return 0;
}
