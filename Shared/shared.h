#ifndef SHARED_H
#define SHARED_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    MEM_COMPLETE,
    MEM_REDUCED,
} Memory_Info_Mode;

typedef struct Memory_Info {
    uint64_t total_memory;
    uint64_t free_memory;
    uint64_t used_memory;
    uint64_t internal_fragmentation;
    uint64_t largest_free_block;
    uint64_t header_size;
    const char *allocator_type;
    Memory_Info_Mode mode;
} Memory_Info;

#endif