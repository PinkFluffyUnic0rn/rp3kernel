#include <stdio.h>
#include <stdlib.h>

#define LINESZ 16

#define PRINTBYTE(b) printf("%c%c", C[(b) >> 4], C[(b) & 0xf]);

char C [] = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
	'9', 'A', 'B', 'C', 'D', 'E', 'F'};

int main(int argc, const char *argv[])
{
	unsigned char buf[LINESZ];	
	FILE *f;
	size_t r;
	size_t addr;

	if ((f = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Cannot open file \"%s\"\n",
			argv[1]);
		return 1;
	}

	addr = 0x0;
	while ((r = fread(buf, 1, LINESZ, f)) > 0) {
		unsigned int s;
		int i;
	
		printf(":");
		PRINTBYTE(LINESZ);
		PRINTBYTE(addr >> 8);
		PRINTBYTE(addr & 0xff);
		PRINTBYTE(0x0);

		for (i = 0; i < LINESZ; ++i)
			PRINTBYTE(buf[i]);

		s = LINESZ + (addr & 0xff) + (addr >> 8);
		for (i = 0; i < LINESZ; ++i)
			s += buf[i];

		s = 0xff & (~s + 1);

		PRINTBYTE(s);
		printf("\r\n");

		addr += LINESZ;
	}

	printf(":00000001FF\r\n");

	if (r < 0) {
		fprintf(stderr, "Cannot read from file %s.\n", argv[1]);
		return 1;
	}

	if (fclose(f) == EOF) {
		fprintf(stderr, "Error while closing file %s.\n",
			argv[1]);
		return 1;
	}

	return 0;
}
