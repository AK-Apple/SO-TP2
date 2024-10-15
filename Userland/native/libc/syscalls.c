#include "../include/syscalls.h"
#include "../include/command.h"

void sys_hlt() {
    syscall(SYS_HLT, 0, 0, 0);
}

int sys_read(int fd, char * buf, int count){
    return syscall(SYS_READ, fd, (void *) buf, count);
}

void sys_write(int fd, char * buf, int count){
    syscall(SYS_WRITE, fd, (void *) buf, count);
}

uint64_t sys_time(int d) {
    return syscall(SYS_TIME, d, 0, 0);
}

void sys_sleep() {
    syscall(SYS_SLEEP, 0, 0, 0);
}

void sys_sound(int frec) {
    syscall(SYS_SOUND, frec, 0, 0);
}

void sys_nosound(){
    syscall(SYS_NOSOUND, 0, 0, 0);
}

void sys_clear(){
    syscall(SYS_CLEAR, 0, 0, 0);
}

void sys_new_size(int newSize){
    syscall(SYS_NEW_SIZE, newSize, 0, 0);
}

void sys_putPixel(uint32_t hexColor, uint64_t x, uint64_t y){
    syscall(SYS_PUT_PIXEL, hexColor, (void *) x, y);
}

int sys_secondsElapsed(){
    return syscall(SYS_GET_ELAPSED_SECONDS, 0, 0, 0);
}

int sys_ticks_elapsed() {
    return syscall(SYS_GET_ELAPSED_TICKS, 0, 0, 0);
}

int sys_getWindowSize(int elem) {
    return syscall(SYS_WIDTH_HEIGHT, elem, 0, 0);
}

void sys_getRegs() {
    syscall(SYS_GETREGS, 0, 0, 0);
}

void *sys_fl_malloc() {
    return syscall(SYS_GET_MEM, 0, 0, 0);
}
#include "../include/stdio.h"

int sys_create_process(Program program, int argc, char **argv) {
    return syscall(SYS_CREATE_PROCESS, program, argc, argv);
}
int sys_kill_process(uint64_t pid) {
    return syscall(SYS_KILL_PROCESS, pid, 0, 0);
}
int sys_get_process_status(int pid) {
    return syscall(SYS_PROCESS_STATUS, pid, 0, 0);
}
int sys_get_pid() {
    return syscall(SYS_GET_PID, 0, 0, 0);
}
void sys_print_all_processes() {
    syscall(SYS_GET_ALL_PROCESSES, 0, 0, 0);
}
void sys_change_priority(uint64_t pid, int delta) {
    syscall(SYS_CHANGE_PRIORITY, pid, (uint64_t) delta, 0);
}
int sys_block(int pid) {
    return syscall(SYS_BLOCK, pid, 0, 0);
}
int sys_unblock(int pid) {
    return syscall(SYS_UNLOCK, pid, 0, 0);
}
void sys_yield() {
    syscall(SYS_YIELD, 0, 0, 0);
}
uint64_t sys_wait_pid(uint64_t pid, int *status, int options) {
    return syscall(SYS_WAITPID, pid, status, options);
}
void sys_wait_children() {
    syscall(SYS_WAIT_CHILDREN, 0, 0, 0);
}
void sys_exit(uint64_t return_value) {
    syscall(SYS_EXIT, return_value, 0, 0);
}