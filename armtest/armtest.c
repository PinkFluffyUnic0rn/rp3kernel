#include "calls.h"

int main()
{
	util_sendstr("hello from loaded program!");
	uart_send(0x0D);
	uart_send(0x0A);

	util_sendstr("hello from loaded program!");
	uart_send(0x0D);
	uart_send(0x0A);

	return 0;
}
