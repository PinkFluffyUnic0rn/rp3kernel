#include "periph.h"

struct ih_line {
	unsigned char rsize;
	unsigned int address;
	unsigned char rtype;
	unsigned char data[0xff];
	unsigned char checksum;
};

static int ih_check(struct ih_line *l)
{
	unsigned int s;
	int i;

	s = l->rsize + (l->address & 0xff) + (l->address >> 8 & 0xff)
		+ l->rtype;

	for (i = 0; i < l->rsize; ++i)
		s += l->data[i];

	s &= 0xff;

	if (s != 0)
		s = 0xff & ((~s) + 1);

	if (s != (l->checksum & 0xff))
		return 0;

	return 1;
}

static char ih_getchar()
{	
	int bh, bl;

	bh = uart_recv();
	bl = uart_recv();

	if (bh >= 0x30 && bh <= 0x39)
		bh -= 0x30;
	else if (bh >= 0x41 && bh <= 0x46) {
		bh -= 0x41;
		bh += 10;
	}

	if (bl >= 0x30 && bl <= 0x39)
		bl -= 0x30;
	else if (bl >= 0x41 && bl <= 0x46) {
		bl -= 0x41;
		bl += 10;
	}

	return ((bh & 0x0000000f) << 4) | (bl & 0x0000000f);
}

int ih_getfile(unsigned char *dest)
{
	struct ih_line l;
	unsigned char offset;
	int i, j;

	uart_init();

	offset = 0;
	while (1) {
		// get colon
		uart_recv();

		// get record size	
		l.rsize = ih_getchar();

		// get address
		l.address = (ih_getchar() & 0xff) << 8;
		l.address |= (ih_getchar() & 0xff);

		// get record type
		l.rtype = ih_getchar();
	
		// get data
		for (i = 0; i < l.rsize; ++i)
			l.data[i] = ih_getchar();
	
		// get checksum
		l.checksum = ih_getchar();

		// get CL/LF
		uart_recv();
		uart_recv();

		if (ih_check(&l) == 0) {
			uart_send('n');
			continue;
		}

		uart_send('y');

		if (l.rtype == 0x00) {
			unsigned char *d;

			d = dest + offset + l.address;

			for (j = 0; j < l.rsize; ++j)
				d[j] = l.data[j];
		}
		else if (l.rtype == 0x01)
			break;
		else if (l.rtype == 0x02)
			offset = (l.data[0] << 12 | l.data[1] << 4);
		else if (l.rtype == 0x04)
			offset = (l.data[0] << 24 | l.data[1] << 16);
	}

	return 0;
}
