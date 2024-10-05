#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#include "util.h"
#include "periph.h"

#define MEMSTART ((unsigned char *) 0x00008000)
#define MEMEND ((unsigned char *) 0x10000000)

unsigned char *membreak = MEMSTART;

static unsigned char *heapstart = ((unsigned char *) MEMEND);
static struct block *freehead;

struct block {
	size_t size;
	struct block *p;
	struct block *n;
};

void sendstr(char *s)
{
	char *p;

	for (p = s; *p != '\0'; ++p)
		uart_send(*p);
}

void uint2hexstr(unsigned int c, char *s)
{
	char t[] = {'0', '1', '2', '3', '4', '5',
		'6', '7', '8', '9', 'a', 'b', 'c',
		'd', 'e', 'f', '?'};
	int i;

	for (i = 0; i < sizeof(unsigned int) * 8; i += 4) {
		int h;

		h = (c >> (sizeof(unsigned int) * 8 - i - 4)) & 0xf;

		if (h > 15) h = 16;

		s[i / 4] = t[h];
	}

	s[(sizeof(unsigned int) * 8) / 4] = '\0';
}

void uint2str(unsigned int u, char *s)
{
	char *c;
	char buf[64];

	c = buf;
	while (u != 0) {
		*c++ = (u % 10) + '0';
		u /= 10;
	}

	while (c != buf)
		*s++ = *(--c);

	*s = '\0';
}

void sendhexint(unsigned int u)
{
	char buf[1024];

	uint2hexstr(u, buf);

	sendstr(buf);
}

void senduint(unsigned int u)
{
	char buf[1024];

	uint2str(u, buf);

	sendstr(buf);
}

void sendint(int i)
{
	char buf[1024];

	uint2str(i, buf);

	sendstr(buf);
}

int printf(const char *format, ...)
{
	va_list vl;
	const char *c;

	va_start(vl, format);

	for (c = format; *c != '\0'; ++c) {
		if (*c == '%') {
			++c;
			switch (*c) {
			case 'u':
				senduint(va_arg(vl, uint32_t));
				break;
			case 'd':
				sendint(va_arg(vl, int32_t));
				break;
			case 'x':
				sendstr("0x");
				sendhexint(va_arg(vl, uint32_t));
				break;
			case 's':
				sendstr(va_arg(vl, char *));
				break;
			case '%':
				uart_send('%');
				break;
			case '\0':
				break;
			}
		}
		else {
			uart_send(*c);
		}
	}

	va_end(vl);

	return 0;
}

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
