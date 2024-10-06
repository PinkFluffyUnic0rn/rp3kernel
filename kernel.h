#ifndef KERNEL_H
#define KERNEL_H

int kernel();

int undefined();

int swi();

int prefetch();

int datafault(uint32_t pc, uint32_t status, uint32_t addr);

int hyp();

int irq();

int fiq();

#endif
