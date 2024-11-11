#ifndef TPE_ARQUI_IO_H
#define TPE_ARQUI_IO_H

#include <stdint.h>
#include "shared.h"

#define STD_FILE_DESCRIPTORS ((fd_t)(3))
#define EOF ((int)(-1))

#define COLOR_RED       0x00FF0000
#define COLOR_YELLOW    0x00CDFF00
#define COLOR_BLUE      0x000000FF
#define COLOR_GREEN     0x0000FF00
#define COLOR_ORANGE    0x00FF8800

void put_out(char c);
void printf(char * fmt, ...);
void sys_write(int fd, const char* buf, int count);
int sys_read(int fd, char* buf, int count);
void sys_new_size(int newSize);
void sys_clear_screen();
void set_foreground_color(uint64_t fg);
void printf_error(char * fmt, ...);
void printf_color(char * fmt, uint64_t foreground, ...);

#endif //TPE_ARQUI_IO_H
