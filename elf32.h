#ifndef ELF32_H
#define ELF32_H

#include <stddef.h>

struct procinfo {
	void (*entry)();
	unsigned char *img;
	size_t imgsz;
};

int Elf32_load(unsigned char *elf, unsigned char *start,
	unsigned char *maxsz, struct procinfo *pinfo);

#endif
