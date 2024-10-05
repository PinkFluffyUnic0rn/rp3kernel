#ifndef KERNEL_H
#define KERNEL_H

int kernel();

int datafault(uint32_t status, uint32_t addr);

#endif
