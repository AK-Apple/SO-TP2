#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <stdint.h>

#define MAX_PROCESS_BLOCKS 64 
#define STACK_SIZE (1 << 12)  /* 4KB */
#define INVALID_PID (-1)
#define MAX_PRIORITY 5
typedef uint64_t Stack[STACK_SIZE];

typedef uint64_t (*Program)(uint64_t, const char*[]);

typedef struct ProcessSnapshot
{
    char *p_name;
    uint64_t pid;
    uint64_t priority;
    Stack stack;
    void *base_pointer;
    uint64_t foreground;
} ProcessSnapshot;

typedef enum
{
    UNAVAILABLE, // Assigned 0 by default
    RUNNING,
    READY,
    BLOCKED
} State;

int create_process(Program program, int argc, char **argv);

void create_init_process();

int kill_process(uint64_t pid);

int64_t get_pid();

int get_process_status(uint64_t pid);

void get_all_processes();

void yield();

void change_priority(uint64_t pid, int value);

int block(int pid);

int unlock(int pid);

void exit(uint64_t return_value);

uint64_t wait_pid(uint64_t pid, int *status, int options);

void children_wait();

void save_regs();

void print_saved_regs();

// -------- SO ----------

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

// ------- Assembler Section ---------

// void _change_process(void);

// -------- end SO -------

#endif