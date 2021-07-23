#include "calls.h"

int main()
{
	char a[1024];
	char *buf;
	int i;
	
	util_sendstr("hello from loaded program!");
	uart_send(0x0D);
	uart_send(0x0A);

	util_sendstr("hello from loaded program!");
	uart_send(0x0D);
	uart_send(0x0A);

	buf = malloc(0x00001000);

	buf[0] = '\0';
	for (i = 0; i < 100; ++i) {
//		util_uint2hexstr(buf, a);
//		util_sendstr(a);

		util_uint2hexstr(i, buf);
		util_sendstr(buf);
		uart_send(0x0D);
		uart_send(0x0A);
	}

	return 0;
}
