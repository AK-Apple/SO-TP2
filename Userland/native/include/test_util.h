#include <stdint.h>
#include "../include/syscalls.h"

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
void bussy_wait(uint64_t n);
int64_t endless_loop(int argc, char* argv[]);
int64_t endless_loop_print(int argc, char* argv[]);

// Getters:

Program get_endless_loop();
Program get_endless_loop_print();
