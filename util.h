#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

void sendstr(char *s);

void uint2hexstr(unsigned int c, char *s);

void uint2str(unsigned int c, char *s);

void sendhexint(unsigned int u);

void senduint(unsigned int u);

void sendint(int i);

int printf(const char *format, ...);

void heapinit();

void *malloc(size_t size);

void free(void *ptr);

#endif
