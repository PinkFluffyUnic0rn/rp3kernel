#include "../calls.h"

#define COUNT 16
#define LOW 1024
#define HIGH (1024 * 32)

int rand(void)
{
	static int next = 1;

	next = next * 1103515245 + 12345;

	return (unsigned int)(next/65536) % 32768;
}

int main()
{
	void *p[COUNT];
	int cnt;

	cnt = 0;
	while (cnt < COUNT) {
		int c;
		int i;

		util_sendstr("count: ");
		util_sendint(cnt);
		util_sendstr("\n\n");

		c = rand() % 5;
		for (i = 0; i < c && cnt < COUNT; ++i) {
			size_t sz;

			sz = rand() % (HIGH - LOW) + LOW;

			util_sendstr("allocating ");
			util_sendhexint(sz);
			util_sendstr(" bytes:\n\n");

			p[cnt++] = malloc(sz);

			util_sendstr("address after malloc: ");
			util_sendhexint(p[cnt - 1]);
			util_sendstr("\n\n");
		}

		c = rand() % 3;
		for (i = 0; i < c && cnt > 0; ++i) {
			util_sendstr("freeing address ");
			util_sendhexint(p[cnt - 1]);
			util_sendstr(":\n\n");

			free(p[--cnt]);
		}
	}

/*
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

	//	util_uint2hexstr(i, buf);
	//	util_sendstr(buf);
	//	uart_send(0x0D);
	//	uart_send(0x0A);
	}

	free(buf);
*/
	return 0;
}
