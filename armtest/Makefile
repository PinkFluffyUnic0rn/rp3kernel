CCPREFIX ?= arm-none-eabi

CFLAGS = -Wall -nostdlib -nostartfiles -ffreestanding 

all : armtest.o
	$(CCPREFIX)-ld -pie -s -e main armtest.o -o armtest
	rm -f *.o

#vectors.o : vectors.s
#	$(CCPREFIX)-as vectors.s -o vectors.o

#periph.o : periph.c 
#	$(CCPREFIX)-gcc $(CFLAGS) -c periph.c -o periph.o

armtest.o : armtest.c
	$(CCPREFIX)-gcc -fPIE -c armtest.c -o armtest.o

clean:
	rm -f *.o
	rm -f armtest
