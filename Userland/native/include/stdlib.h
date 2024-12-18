#ifndef TPE_STDLIB_H
#define TPE_STDLIB_H

#include <stdint.h>

#define MAX_BUF 1024

int getchar();

uint64_t gets(char * buf, uint64_t length);

uint64_t atoi(char * str);
int64_t atoi_index(const char *str, uint64_t *index);
char* itoa(int value, char* result, int base);
uint32_t uint_to_base(uint64_t value, char * buffer, uint32_t base);


void put_pixel(uint32_t hexColor, uint64_t x, uint64_t y);
void put_square(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t size);
char get_key();

void * memset(void * destiation, int32_t c, uint64_t length);

#endif //TPE_STDLIB_H
