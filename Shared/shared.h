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

#define INVALID_PID (-1)
#define MAX_PROCESS_BLOCKS 128 
#define PROCESS_NAME_LEN 32

typedef enum {
    PRIORITY_LOW = 0,
    PRIORITY_MID,
    PRIORITY_HIGH,
    PRIORITY_NONE,
} Priority; 

typedef struct Memory_Info {
    uint64_t base_address;
    uint64_t end_address;
    uint64_t total_memory;
    uint64_t free_memory;
    uint64_t used_memory;
    uint64_t internal_fragmentation;
    uint64_t largest_free_block;
    uint64_t header_size;
    const char *allocator_type;
} Memory_Info;

typedef int64_t pid_t;

typedef enum
{
    BLOCK_ENABLED = 0, // Assigned 0 by default
    BLOCK_DISABLED,
} StdinOption;

typedef struct StackedRegisters
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} StackedRegisters;

typedef enum
{
    STATUS_UNAVAILABLE = 0,
    STATUS_RUNNING,
    STATUS_READY,
    STATUS_BLOCKED
} ProcessStatus;

struct ProcessInfoEntry {
    pid_t pid;
    pid_t ppid;
    Priority priority;
    uint64_t rsp;
    uint64_t rbp;
    ProcessStatus status;
    char name[PROCESS_NAME_LEN];
};

typedef struct {
    uint64_t count;
    pid_t foreground_pid;
    struct ProcessInfoEntry *entries;
} ProcessInfo; 

typedef int64_t (*Program)(uint64_t, char*[]);

typedef struct {
    uint8_t * value;
    int sem_is_empty; 
    int sem_is_full;
} mvar_t;

#endif