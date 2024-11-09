#ifndef SHARED_H
#define SHARED_H

#include <stddef.h>
#include <stdint.h>

#define SEM_SUCCESS 1
#define SEM_ERROR 0

typedef int64_t fd_t;

#define STDIN ((fd_t)0)
#define STDOUT ((fd_t)1)
#define STDERR ((fd_t)2)
#define DEVNULL ((fd_t)(-1L))

typedef enum {
    PRIORITY_LOW = 0,
    PRIORITY_MID,
    PRIORITY_HIGH,
    PRIORITY_NONE,
} Priority; 

typedef enum {
    MEM_COMPLETE = 0,
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

typedef int64_t pid_t;

typedef enum
{
    BLOCK_ENABLED = 0, // Assigned 0 by default
    BLOCK_DISABLED,
} StdinOption;

#endif