#ifndef PROCESS_SO_H
#define PROCESS_SO_H

#include <stdint.h>
#include "shared.h"
#include "registers.h"

#define MAX_FILE_DESCRIPTORS 16
#define STACK_SIZE (1L << 13)  /* 8KB */
typedef uint8_t Stack[STACK_SIZE];

typedef enum {
    NONE = 0,
    KILL_FOREGROUND,
    FOREGROUND_TO_BACKGROUND,
    BLOCK_FOREGROUND,
} PendingAction;

pid_t create_process(Program program, int argc, char **argv, fd_t fds[]);

void create_init_process();

pid_t kill_process(pid_t pid, int recursive);

pid_t get_pid();

int get_process_status(pid_t pid);

void get_all_processes(ProcessInfo *info);

void change_priority(pid_t pid, int value);

pid_t block(pid_t pid);

pid_t block_no_yield(pid_t pid);

pid_t unblock(pid_t pid);

void exit(int64_t return_value);

pid_t wait_pid(pid_t pid);

void sys_set_fd(pid_t pid, fd_t fd_index, fd_t value);

int64_t get_last_exit_code();

void set_pending_action(PendingAction action);

pid_t set_foreground(pid_t pid);

pid_t get_foreground();

fd_t get_fd(fd_t index);

void set_sleeping_state(uint8_t is_sleeping, pid_t pid);

void set_stdin_options(StdinOption stdin_option);

StdinOption get_stdin_options();

// -------- SO ----------

#endif