#ifndef TPE_STDIO_H
#define TPE_STDIO_H

#include <stdint.h>

void scanf(const char * fmt, ...);
uint64_t printf_color(const char * fmt, uint64_t foreground, uint64_t background, ...);
void printf(const char * fmt, ...);
void printInt(int num);
#endif //TPE_STDIO_H