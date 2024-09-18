#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

int buflen(const char *s);
void getRegs();
void sys_registers();

// assembler (libasm.asm)
char * cpuVendor(char *result);

void outb (unsigned char __value, unsigned short int __port);
unsigned char inb (unsigned short int __port);

#endif