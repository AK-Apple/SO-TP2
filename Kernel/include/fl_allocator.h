#ifndef FREE_LIST_MEMORY_ALLOCATOR_H
#define FREE_LIST_MEMORY_ALLOCATOR_H

#include <stdint.h>

#define BLOCK_COUNT 128
#define BLOCK_SIZE 256

void initialize_allocator(void* heap);
void *fl_malloc();
void fl_free(void *pointer);
int fl_total_memory();
void fl_print_mem();


#endif
