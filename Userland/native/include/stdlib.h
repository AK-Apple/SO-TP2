#ifndef TPE_STDLIB_H
#define TPE_STDLIB_H

#include <stdint.h>

#define MAX_BUF 1024

int getchar();

uint64_t gets(char * buf, uint64_t length);

uint64_t atoi(char * str);
char* itoa(int value, char* result, int base);
uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);


void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
void putSquare(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t size);
char getKey();

void * memset(void * destiation, int32_t c, uint64_t length);

#endif //TPE_STDLIB_H
