#ifndef START_H
#define START_H

#include <stdint.h>

#define MAXELFSIZE 0x00100000
#define MAXPROCN 128

#define LOADER		0x00000000
#define KERNELINIT 	0x00100000
#define USERSPACE	0x08048000
#define KERNEL 		0x3c000000
#define PAGETABLES	0x3d000000
#define CALLTABLE 	0x3e000000
#define PBASE 		0x3f000000
#define ADDREND 	0x40000000

#define LOADERSIZE 0x100000
#define SECTIONSIZE 0x100000
#define KERNELSIZE 0x100000

int mmuenable(uint32_t);

int mmudisable();

int invalidatetlbs();

uint32_t getpc();

#endif
