#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

uint64_t get_rip();
uint64_t process_initializer(char* name, int argc, char* argv[]);

#endif