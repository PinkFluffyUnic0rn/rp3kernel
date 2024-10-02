#ifndef UTIL_H
#define UTIL_H

void util_sendstr(char *s);

void util_uint2hexstr(unsigned int c, char *s);

void util_uint2str(unsigned int c, char *s);

void util_sendhexint(unsigned int u);

void util_senduint(unsigned int u);

void util_sendint(int i);

int util_printf(const char *format, ...);

#endif
