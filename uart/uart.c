#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <wait.h>
#include <string.h>

int ttyset(int fd, struct termios *prevterm)
{
	struct termios t;

	if (tcgetattr(fd, &t) < 0) {
		fprintf(stderr, "Cannot get serial attributes.\n");
		return 1;
	}

	if (prevterm != NULL)
		*prevterm = t;

	cfmakeraw(&t);

	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;

	if (cfsetispeed(&t, B115200) < 0) {
		fprintf(stderr, "Cannot input baud rate.\n");
		return (-1);
	}

	if (cfsetospeed(&t, B115200) < 0) {
		fprintf(stderr, "Cannot output baud rate.\n");
		return (-1);
	}

	if (tcsetattr(fd, TCSAFLUSH, &t) < 0) {
		fprintf(stderr, "Cannot set serial attributes.\n");
		return (-1);
	}

	return 0;
}

int sendihex(FILE *ihex, int serial)
{
	char *s;
	ssize_t ssz;
	ssize_t slen;

	s = NULL;
	while ((slen = getline(&s, &ssz, ihex)) > 0) {
		struct timespec t;
		char *p;
		ssize_t r;
		char ack;

		if (*s != ':')
			continue;
corrupt:
		for (p = s; *p != '\0'; ++p)
			write(serial, p, 1);

		if ((r = read(serial, &ack, 1)) < 0) {
			fprintf(stderr,
				"Error while reading an answer.\n");
			return (-1);

		}

//		printf("c = %c\n", ack);
//		fflush(stdout);

		if (ack != 'y')
			goto corrupt;
	}

	char c;
	ssize_t r;
	while ((r = read(serial, &c, 1)) >= 0) {
		printf("%c", c);
		fflush(stdout);
	}


	free(s);

	return 0;
}

int main(int argc, char *argv[])
{
	struct termios t;
	int serial;
	FILE *ihex;
	char c;
	int r;

	if (argc < 3) {
		fprintf(stderr, "Not enough arguments.\n");
		return 1;
	}

	if ((serial = open(argv[1], O_RDWR | O_NOCTTY | O_SYNC)) < 0) {
		fprintf(stderr, "Cannot open serial device.\n");
		return 1;
	}
	
	if ((ihex = fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "Cannot open intel hex file.\n");
		return 1;
	}
	
	if (ttyset(serial, &t) < 0)
		return 1;
	
	sendihex(ihex, serial);

	if (tcsetattr(serial, TCSAFLUSH, &t) < 0) {
		fprintf(stderr, "Cannot restore serial attributes.\n");
		return 1;
	}

	close(serial);

	return 0;
}
