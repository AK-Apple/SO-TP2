#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <stdint.h>

#define MAX_PROCESS_BLOCKS 64 /* <--- esto puede ir en .h */
#define STACK_SIZE (1 << 12) /* 4KB */

void create_process(uint64_t parent_pid, char **argv);

void kill_process(uint64_t pid);

void store_context();

#endif