#ifndef TPE_STDIO_H
#define TPE_STDIO_H

#include <stdint.h>

#define COLOR_RED       0x00FF0000
#define COLOR_YELLOW    0x00CDFF00
#define COLOR_BLUE      0x000000FF
#define COLOR_GREEN     0x0000FF00
#define COLOR_ORANGE    0x00FF8800

#define EOF (-1)


void scanf(char * fmt, ...);
uint64_t printf_color(char * fmt, uint64_t foreground, uint64_t background, ...);
void printf(char * fmt, ...);
void repeat_char(int fd, char c, int count);
void putchar(char c);
void putcharColoured(char c, uint64_t foreground, uint64_t background);
void printf_error(char *fmt, ...);
void fputchar(char c, int fd);
void fprinf(int fd, char *fmt, ...);
uint64_t fprintf_color(int fd, char * fmt, uint64_t foreground, uint64_t background, ...);

#endif //TPE_STDIO_H