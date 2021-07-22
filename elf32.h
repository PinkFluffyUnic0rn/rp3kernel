#ifndef ELF32_H
#define ELF32_H

#include <stddef.h>

struct procinfo {
	unsigned char *entry;
	unsigned char *img;
	size_t imgsz;
};

int Elf32_load(unsigned char *elf, unsigned char *start,
	size_t maxsz, struct procinfo *pinfo);

#endif
