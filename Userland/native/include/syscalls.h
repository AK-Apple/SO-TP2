#ifndef TPE_ARQUI_SYSCALLS_H
#define TPE_ARQUI_SYSCALLS_H

#include "./../../../Shared/shared.h"

#include <stdint.h>
#include <stddef.h>

typedef int64_t (*Program)(uint64_t, char*[]);

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
extern void sys_putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
extern int sys_ticks_elapsed();
extern int sys_secondsElapsed();

extern void sys_new_size(int newSize);
extern int sys_getWindowSize(int elem);

extern void sys_getRegs();

extern int sys_create_process_fd(Program program, int argc, char **argv, int fds[]);
extern int sys_create_process(Program program, int argc, char **argv);
extern int sys_kill_process(uint64_t pid);
extern int sys_get_process_status(int pid);
extern int sys_get_pid();
extern void sys_print_all_processes();
extern void sys_change_priority(uint64_t pid, uint64_t delta);
extern int sys_block(int pid);
extern int sys_unblock(int pid);
extern void sys_yield();
extern uint64_t sys_wait_pid(uint64_t pid);
extern void sys_set_fd(int pid, int fd_index, int value);
extern void sys_exit(uint64_t return_value);

extern void *sys_memory_alloc(size_t bytes);
extern void sys_memory_free(void *pointer);
extern void sys_memory_info(Memory_Info *info, Memory_Info_Mode mode);
// size_t sys_memory_largest_block();

extern int sys_sem_open(int64_t path, int init);
extern int sys_sem_wait(int64_t path);
extern int sys_sem_post(int64_t path);
extern int sys_sem_close(int64_t path);

extern int sys_create_pipe(int fd);
extern int sys_request_pipe();
extern int sys_close_pipe(int fd);

#endif //TPE_ARQUI_SYSCALLS_H
