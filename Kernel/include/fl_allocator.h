#ifndef FREE_LIST_MEMORY_ALLOCATOR_H
#define FREE_LIST_MEMORY_ALLOCATOR_H

#include <stdint.h>

#define BLOCK_COUNT 64
#define BLOCK_SIZE 256
#define TOTAL_MEMORY_BYTES BLOCK_COUNT * BLOCK_SIZE
#define MANAGED_MEMORY_ADDRESS ((void*)0xff)

void initialize_allocator();
void *fl_malloc();
void fl_free(void *pointer);

#endif
