#include <stdint.h>

#include "util.h"
#include "periph.h"

void util_sendstr(char *s)
{
	char *p;

	for (p = s; *p != '\0'; ++p)
		uart_send(*p);
}

void util_uint2hexstr(unsigned int c, char *s)
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

void util_uint2str(unsigned int u, char *s)
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

void util_sendhexint(unsigned int u)
{
	char buf[1024];

	util_uint2hexstr(u, buf);

	util_sendstr(buf);
}

void util_senduint(unsigned int u)
{
	char buf[1024];

	util_uint2str(u, buf);

	util_sendstr(buf);
}

void util_sendint(int i)
{
	char buf[1024];

	util_uint2str(i, buf);

	util_sendstr(buf);
}
