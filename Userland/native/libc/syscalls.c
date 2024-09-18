#include "../include/syscalls.h"


void sys_hlt() {
    syscall(SYS_HLT, 0, 0, 0);
}

int sys_read(int fd, char * buf, int count){
    return syscall(SYS_READ, fd, buf, count);
}

void sys_write(int fd, char * buf, int count){
    syscall(SYS_WRITE, fd, buf, count);
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
    syscall(SYS_PUT_PIXEL, hexColor, x, y);
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