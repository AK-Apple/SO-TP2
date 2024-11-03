// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/stdio.h"
#include <stdarg.h>
#include "../include/stdlib.h"
#include "../include/syscalls.h"
#include "string.h"

#define MAX_BUF 1024

static uint64_t default_foreground_color = 0x00FFFFFF;

static uint64_t vfprintf_color(int fd, char *fmt, uint64_t foreground, uint64_t background, va_list vars);

void putchar(char c) {
    char buf[] = {c, 0};
    sys_write(STD_OUT, buf, 1);
}

void fputchar(char c, int fd) {
    char buf[] = {c, 0};
    sys_write(fd, buf, 1);
}

void putcharColoured(char c, uint64_t foreground, uint64_t background) {
    if(foreground != default_foreground_color) {
        sys_set_text_color(foreground);
    }
    
    putchar(c);

    if(foreground != default_foreground_color) {
        sys_set_text_color(default_foreground_color);
    }
}

void repeat_char(char c, int count) {
    while(count-- > 0) {
        putchar(c);
    }
}

void scanf(char * fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buf[MAX_BUF];

    for (int i = 0; fmt[i] != '\0' ; i++) {
        if (fmt[i++] == '%') {
            switch (fmt[i]) {
                case 's':
                    gets(va_arg(args, char *), MAX_BUF);
                    break;
                case 'c':
                    gets(va_arg(args, char *), 1);
                    break;
                case 'd':
                    gets(buf, MAX_BUF);
                    int value = atoi(buf);
                    int * aux = va_arg(args, int *);
                    * aux = value;
                    break;
                default:
                    break;
            }
        }
    }
    va_end(args);
}

uint64_t printf_color(char * fmt, uint64_t foreground, uint64_t background, ...) {
    va_list args;
    va_start(args, background);  

    uint64_t i = vfprintf_color(STD_OUT, fmt, foreground, background, args);

    va_end(args);  
    return i;
}

uint64_t fprintf_color(int fd, char * fmt, uint64_t foreground, uint64_t background, ...) {
    va_list args;
    va_start(args, background);  

    uint64_t i = vfprintf_color(fd, fmt, foreground, background, args);

    va_end(args);  
    return i;
}

void printf_error(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf_color(STD_ERR, fmt, default_foreground_color, 0x000000, args); 
    va_end(args);
}

static uint64_t vfprintf_color(int fd, char *fmt, uint64_t foreground, uint64_t background, va_list vars) {
    uint64_t i = 0;

    if(foreground != default_foreground_color) {
        sys_set_text_color(foreground);
    }

    while (fmt[i]) {
        if (fmt[i] == '%') {
            char buf[MAX_BUF] = {0};
            uint64_t len = 0;
            switch (fmt[++i]) {
                case 'u':
                    len = uintToBase(va_arg(vars, int), buf, 10);
                    break;
                case 'd':
                    len = (uint64_t)(itoa(va_arg(vars, int64_t), buf, 10) - buf);
                    break;
                case 'x':
                    len = uintToBase(va_arg(vars, int), buf, 16);
                    break;
                case 'b':
                    len = uintToBase(va_arg(vars, int), buf, 2);
                    break;
                case 'c':
                    len = 1;
                    buf[0] = va_arg(vars, int);
                    break;
                case 's':;
                    char *str = va_arg(vars, char*);
                    len = strlen(str);
                    sys_write(fd, str, len);
                    i++;
                    continue;
                case '%':
                    len = 1;
                    buf[0] = '%';
                    break;
                default:
                    break;
            }
            buf[len] = '\0';
            sys_write(fd, buf, len);
        } else {
            sys_write(fd, &fmt[i], 1);
        }
        i++;
    }

    if(foreground != default_foreground_color) {
        sys_set_text_color(default_foreground_color);
    }
    return i;
}

void printf(char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf_color(STD_OUT, fmt, default_foreground_color, 0x000000, args);
    va_end(args);
}

void fprinf(int fd, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf_color(fd, fmt, default_foreground_color, 0x000000, args);
    va_end(args);
}

void printInt(int num) {
    printf("%d", num);
}
