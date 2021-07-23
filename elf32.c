#include "elf32.h"

#include <stdint.h>
#include <stddef.h>

#define BUFSIZE 4096
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define EI_NIDENT 16

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

struct Elf32_Ehdr {
	unsigned char e_ident[EI_NIDENT];
	Elf32_Half e_type;
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry;
	Elf32_Off e_phoff;
	Elf32_Off e_shoff;
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
};

struct Elf32_Phdr {
	Elf32_Word p_type;
	Elf32_Off p_offset;
	Elf32_Addr p_vaddr;
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz;
	Elf32_Word p_memsz;
	Elf32_Word p_flags;
	Elf32_Word p_align;
};

struct Elf32_Shdr {
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
};

enum Elf32_ET {
	ET_NONE = 0,
	ET_REL = 1,
	ET_EXEC = 2,
	ET_DYN = 3,
	ET_CORE = 4,
	ET_LOPROC = 0xff00,
	ET_HIPROC = 0xffff
};

enum Elf32_EV {
	EV_NONE = 0,
	EV_CURRENT = 1
};

enum Elf32_SHT {
	SHT_NULL = 0,
	SHT_PROGBITS = 1,
	SHT_SYMTAB = 2,
	SHT_STRTAB = 3,
	SHT_RELA = 4,
	SHT_HASH = 5,
	SHT_DYNAMIC = 6,
	SHT_NOTE = 7,
	SHT_NOBITS = 8,
	SHT_REL = 9,
	SHT_SHLIB = 10,
	SHT_DYNSYM = 11,
	SHT_LOPROC = 0x70000000,
	SHT_HIPROC = 0x7fffffff,
	SHT_LOUSER = 0x80000000,
	SHT_HIUSER = 0xffffffff
};

enum Elf32_PT {
	PT_NULL = 0,
	PT_LOAD = 1,
	PT_DYNAMIC = 2,
	PT_INTERP = 3,
	PT_NOTE = 4,
	PT_SHLIB = 5,
	PT_PHDR = 6,
	PT_LOPROC = 0x70000000,
	PT_HIPROC = 0x7fffffff
};

struct Elf32_Rel {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
};

struct Elf32_Rela {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
	Elf32_Sword r_addend;
};

enum Elf32_R_ARM {
	R_ARM_RELATIVE = 23
};

static void memcpy(void *dest, const void *src, size_t n)
{
	int i;

	for (i = 0; i < n; ++i) {
		*((unsigned char *) dest++)
			= *((unsigned char *) src++);
	}
}

static int Elf32_relocation(struct Elf32_Shdr *shdr, unsigned char *elf,
	unsigned char *start)
{
	int c, j;
	int s;

	c = 0;
	for (s = 0; s < shdr->sh_size; s += shdr->sh_entsize)
		++c;

	for (j = 0; j < c; ++j) {
		int32_t *p;

		if (shdr->sh_type == SHT_REL) {
			struct Elf32_Rel *r;
		
			r = (struct Elf32_Rel *) (elf 
				+ shdr->sh_offset
				+ sizeof(struct Elf32_Rel) * j);

			p = (int32_t *) (start + r->r_offset);

			*p += (int32_t) start;

			if ((unsigned char) r->r_info
				!= R_ARM_RELATIVE) {
				continue;
			}
		}
		else {
			/*
			struct Elf32_Rela *ra;

			r = (struct Elf32_Rel *) (elf 
				+ shdr->sh_offset
				+ sizeof(struct Elf32_Rel) * j);
			*/
		}
	}

	return 0;
}

int Elf32_load(unsigned char *elf, unsigned char *start,
	unsigned char *memend, struct procinfo *pinfo)
{
	struct Elf32_Ehdr *hdr;
	int i;

	hdr = (struct Elf32_Ehdr *) elf;

	// loading segments into memory
	pinfo->entry = (void (*)()) start + hdr->e_entry;
	pinfo->img = start;
	pinfo->imgsz = 0;
	for (i = 0; i < hdr->e_phnum; ++i) {
		struct Elf32_Phdr *phdr;

		phdr = (struct Elf32_Phdr *) (elf + hdr->e_phoff
			 + sizeof(struct Elf32_Phdr) * i);

		if (start + phdr->p_vaddr + phdr->p_memsz > memend)
			return (-1);

		memcpy(start + phdr->p_vaddr,
			elf + phdr->p_offset, phdr->p_memsz);

		if (phdr->p_vaddr + phdr->p_memsz > pinfo->imgsz)
			pinfo->imgsz = phdr->p_vaddr + phdr->p_memsz;
	}

	// relocating symbols
	for (i = 0; i < hdr->e_shnum; ++i) {
		struct Elf32_Shdr *shdr;

		shdr = (struct Elf32_Shdr *) (elf + hdr->e_shoff
			+ sizeof(struct Elf32_Shdr) * i);

		if (shdr->sh_type != SHT_REL
			&& shdr->sh_type != SHT_RELA)
			continue;

		if (Elf32_relocation(shdr, elf, start) < 0)
			return (-1);
	}

	return 0;
}
