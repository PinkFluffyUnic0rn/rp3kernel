CCPREFIX ?= arm-none-eabi
CFLAGS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding 

all: start.o util.o periph.o kernel.o elf32.o ihex.o
	$(CCPREFIX)-ld $^ -T loader -o kernel.elf
	$(CCPREFIX)-objcopy kernel.elf -O binary kernel7.img
	rm *.o
	rm kernel.elf

start.o: start.s
	$(CCPREFIX)-as start.s -o start.o

kernel.o : kernel.c
	$(CCPREFIX)-gcc $(CFLAGS) -c $^ -o $@

util.o : util.c 
	$(CCPREFIX)-gcc $(CFLAGS) -c $^ -o $@

periph.o : periph.c 
	$(CCPREFIX)-gcc $(CFLAGS) -c $^ -o $@

elf32.o : elf32.c
	$(CCPREFIX)-gcc $(CFLAGS) -c $^ -o $@

ihex.o : ihex.c
	$(CCPREFIX)-gcc $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.list
	rm -f *.img
