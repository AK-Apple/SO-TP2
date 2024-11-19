#ifndef TPE_ARQUI_SYSCALLS_H
#define TPE_ARQUI_SYSCALLS_H

#include "shared.h"

#include <stdint.h>
#include <stddef.h>

extern void sys_hlt();

extern int sys_read(int fd, char * buf, int count);
extern void sys_write(int fd, char* buf, int count);
extern void sys_set_text_color(uint64_t fg);

extern uint64_t sys_time(int d);
extern void sys_test(int a, int b, int c, int d, int e);
extern void sys_sleep(uint64_t seconds);

extern void sys_sound(int frec);
extern void sys_nosound();

extern void sys_clear();

extern void print_help();
extern void sys_put_pixel(uint32_t hex_color, uint64_t x, uint64_t y);
extern int sys_ticks_elapsed();
extern int sys_seconds_elapsed();

extern void sys_new_size(int newSize);
extern int sys_get_window_size(int elem);

extern void sys_get_registers(StackedRegisters *registers);

extern int sys_create_process_fd(Program program, int argc, char **argv, fd_t fds[]);
extern int sys_create_process(Program program, int argc, char **argv);
extern int sys_kill_process(uint64_t pid, int recursive);
extern int sys_get_process_status(int pid);
extern int sys_get_pid();
extern void sys_get_processes_info(ProcessInfo *info);
extern void sys_change_priority(uint64_t pid, uint64_t delta);
extern int sys_block(int pid);
extern int sys_unblock(int pid);
extern void sys_yield();
extern uint64_t sys_wait_pid(uint64_t pid);
extern void sys_set_fd(int pid, fd_t fd_index, fd_t value);
extern void sys_exit(uint64_t return_value);
extern int64_t sys_get_exit_code();
extern pid_t sys_set_foreground(pid_t pid);

extern void *sys_memory_alloc(size_t bytes);
extern void sys_memory_free(void *pointer);
extern void sys_memory_info(Memory_Info *info);

extern int sys_sem_open(int64_t path, int init);
extern int sys_sem_wait(int64_t path);
extern int sys_sem_post(int64_t path);
extern int sys_sem_close(int64_t path);

extern int8_t sys_create_pipe(fd_t* fd_buffer);
extern int sys_close_pipe(fd_t fd);

extern void sys_ticks_sleep(uint64_t ticks);
extern void sys_set_stdin_options(StdinOption stdin_option);

void sys_openMVar(mvar_t * mvar);
void sys_putMVar(mvar_t * mvar, int value);
int sys_takeMVar(mvar_t * mvar);

#endif //TPE_ARQUI_SYSCALLS_H
