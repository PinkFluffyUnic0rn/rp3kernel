CCPREFIX ?= arm-none-eabi
CFLAGS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding -fPIE -static-pie

all: start.o util.o periph.o loader.o elf32.o ihex.o regs.o kernel.o
	$(CCPREFIX)-ld -pie --no-dynamic-linker -Map=output.map $^ -T loader -o kernel.elf
	$(CCPREFIX)-objcopy kernel.elf -O binary kernel7.img
	$(CCPREFIX)-objdump -D kernel.elf >kernel.s
	rm *.o
	rm kernel.elf

start.o: start.s
	$(CCPREFIX)-as start.s -o start.o

regs.o: regs.s
	$(CCPREFIX)-as regs.s -o regs.o

kernel.o : kernel.c
	$(CCPREFIX)-gcc $(CFLAGS) -c $^ -o $@

loader.o : loader.c
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
