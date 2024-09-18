#ifndef TPE_ARQUI_IO_H
#define TPE_ARQUI_IO_H

#include <stdint.h>


void putOut(char c);
void putIn(char c);
void putChar(char c);
void printf(char * str);
void sys_write(int fd, const char* buf, int count);
int sys_read(int fd, char* buf, int count);
void sys_new_size(int newSize);
void sys_clearScreen();






#endif //TPE_ARQUI_IO_H
