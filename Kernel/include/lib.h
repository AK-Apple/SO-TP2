#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <stddef.h>

#define MAX_BUF_SIZE 1024

uint32_t uint_to_base(uint64_t value, char * buffer, uint32_t base);

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

int buflen(const char *s);

// assembler (libasm.asm)
extern char * cpuVendor(char *result);
extern void force_timer_tick();
extern void setup_kernel_restart();

extern void outb (unsigned char __value, unsigned short int __port);
extern unsigned char inb (unsigned short int __port);

extern void acquire(uint8_t * lock);
extern void release(uint8_t * lock);


// -------- Funciones auxiliares para el kernel ---------

int k_strcmp(const char *str1, const char *str2);

void k_print_integer(uint64_t number, uint64_t padding, uint64_t base);

int64_t k_atoi(const char* str);
int64_t k_atoi_index(const char *str, uint64_t *index);
unsigned int log(uint64_t n, int base);

#endif