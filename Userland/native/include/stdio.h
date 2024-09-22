#ifndef TPE_STDIO_H
#define TPE_STDIO_H

#include <stdint.h>

void scanf(char * fmt, ...);
uint64_t printf_color(char * fmt, uint64_t foreground, uint64_t background, ...);
void printf(char * fmt, ...);
void printInt(int num);
#endif //TPE_STDIO_H