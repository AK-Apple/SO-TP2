#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H
// #define USE_BUDDY_ALLOCATOR // -D=USE_BUDDY_ALLOCATOR

#include <stdint.h>
#include <stddef.h>

void initialize_memory_allocator(void *base_address, uint64_t total_bytes);
void *memory_alloc(size_t bytes);
void memory_free(void *pointer);
void memory_info();
size_t largest_free_block();

#endif
