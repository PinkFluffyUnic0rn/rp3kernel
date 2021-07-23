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
