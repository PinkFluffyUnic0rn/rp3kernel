#include <stdint.h>
#include <stdarg.h>

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

int util_printf(const char *format, ...)
{
	va_list vl;
	const char *c;

	va_start(vl, format);

	for (c = format; *c != '\0'; ++c) {
		if (*c == '%') {
			++c;
			switch (*c) {
			case 'u':
				util_senduint(va_arg(vl, uint32_t));
				break;
			case 'd':
				util_sendint(va_arg(vl, int32_t));
				break;
			case 'x':
				util_sendstr("0x");
				util_sendhexint(va_arg(vl, uint32_t));
				break;
			case 's':
				util_sendstr(va_arg(vl, char *));
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
