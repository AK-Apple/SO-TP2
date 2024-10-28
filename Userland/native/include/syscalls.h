#ifndef TPE_ARQUI_SYSCALLS_H
#define TPE_ARQUI_SYSCALLS_H

#include "./../../../Shared/shared.h"

#define SYS_HLT 0
#define SYS_SOUND 1
#define SYS_NOSOUND 12
#define SYS_SLEEP 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_CLEAR 5
#define SYS_PUT_PIXEL 6
#define SYS_NEW_SIZE 7
#define SYS_TIME 8
#define SYS_GET_ELAPSED_SECONDS 9
#define SYS_GET_ELAPSED_TICKS 10
#define SYS_WIDTH_HEIGHT 11
#define SYS_GETREGS 13
#define SYS_SET_TEXT_COLOR 14

#define SYS_CREATE_PROCESS 15
#define SYS_KILL_PROCESS 16
#define SYS_PROCESS_STATUS 17
#define SYS_GET_PID 18
#define SYS_GET_ALL_PROCESSES 19
#define SYS_CHANGE_PRIORITY 20
#define SYS_BLOCK 21
#define SYS_UNLOCK 22
#define SYS_YIELD 23
#define SYS_WAITPID 24
#define SYS_WAIT_CHILDREN 25
#define SYS_EXIT 26

#define SYS_MALLOC 27
#define SYS_FREE 28
#define SYS_PRINT_MEM 29
// #define SYS_LARGEST_FREE_BLOCK 30

#define SYS_SEM_OPEN 31
#define SYS_SEM_WAIT 32
#define SYS_SEM_POST 33
#define SYS_SEM_CLOSE 34

#include <stdint.h>
#include <stddef.h>

typedef uint64_t (*Program)(uint64_t, char*[]);

extern uint64_t syscall(uint64_t id, uint64_t par2, uint64_t par3, uint64_t par4);

void sys_hlt();

int sys_read(int fd, char * buf, int count);
void sys_write(int fd, char* buf, int count);
void sys_set_text_color(uint64_t fg);

uint64_t sys_time(int d);

void sys_sleep(uint64_t seconds);

void sys_sound(int frec);
void sys_nosound();

void sys_clear();

void print_help();
void sys_putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
int sys_ticks_elapsed();
int sys_secondsElapsed();

void sys_new_size(int newSize);
int sys_getWindowSize(int elem);

void sys_getRegs();

int sys_create_process(Program program, int argc, char **argv);
int sys_kill_process(uint64_t pid);
int sys_get_process_status(int pid);
int sys_get_pid();
void sys_print_all_processes();
void sys_change_priority(uint64_t pid, uint64_t delta);
int sys_block(int pid);
int sys_unblock(int pid);
void sys_yield();
uint64_t sys_wait_pid(uint64_t pid);
void sys_wait_children();
void sys_exit(uint64_t return_value);

void *sys_memory_alloc(size_t bytes);
void sys_memory_free(void *pointer);
void sys_memory_info(Memory_Info *info, Memory_Info_Mode mode);
// size_t sys_memory_largest_block();

int sys_sem_open(int64_t path, int init);
int sys_sem_wait(int64_t path);
int sys_sem_post(int64_t path);
int sys_sem_close(int64_t path);

#endif //TPE_ARQUI_SYSCALLS_H
