#ifndef LIB_H
#define LIB_H

#include <stdint.h>

#define MAX_BUF_SIZE 1024

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

int buflen(const char *s);
void getRegs();
void sys_registers();

// assembler (libasm.asm)
char * cpuVendor(char *result);

void outb (unsigned char __value, unsigned short int __port);
unsigned char inb (unsigned short int __port);


// -------- Funciones auxiliares para el kernel ---------

#define NULL 0

int k_strcmp(const char *str1, const char *str2);

void k_print_int_dec(int64_t number);

int64_t k_atoi(const char* str);

#endif