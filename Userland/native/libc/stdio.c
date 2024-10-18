#include "../include/stdio.h"
#include <stdarg.h>
#include "../include/stdlib.h"

#define MAX_BUF 1024

static uint64_t default_foreground_color = 0x00FFFFFF;

static uint64_t vprintf_color(char *fmt, uint64_t foreground, uint64_t background, va_list vars);
static void print_str(const char * str);

void putcharColoured(char c, uint64_t foreground, uint64_t background) {
    if(foreground != default_foreground_color) {
        sys_set_text_color(foreground);
    }
    
    putchar(c);

    if(foreground != default_foreground_color) {
        sys_set_text_color(default_foreground_color);
    }
}

void putchar(char c) {
    char buf[1] = {c};
    sys_write(1, buf, 1);
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

static void print_str(const char * str) {
    uint64_t i = 0;
    while (str[i]) {
        putchar(str[i++]);
    }
}

uint64_t printf_color(char * fmt, uint64_t foreground, uint64_t background, ...) {
    va_list args;
    va_start(args, background);  

    uint64_t i = vprintf_color(fmt, foreground, background, args);

    va_end(args);  
    return i;
}

void printf_error(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf_color(fmt, COLOR_RED, 0x000000, args);
    va_end(args);
}

static uint64_t vprintf_color(char *fmt, uint64_t foreground, uint64_t background, va_list vars) {
    uint64_t i = 0;

    if(foreground != default_foreground_color) {
        sys_set_text_color(foreground);
    }

    while (fmt[i]) {
        if (fmt[i] == '%') {
            char buf[MAX_BUF] = {0};
            switch (fmt[++i]) {
                case 'd':
                    uintToBase(va_arg(vars, int), buf, 10);
                    print_str(buf);
                    break;
                case 'x':
                    uintToBase(va_arg(vars, int), buf, 16);
                    print_str(buf);
                    break;
                case 'b':
                    uintToBase(va_arg(vars, int), buf, 2);
                    print_str(buf);
                    break;
                case 'c':
                    putchar(va_arg(vars, int));
                    break;
                case 's':
                    print_str(va_arg(vars, char*));
                    break;
                case '%':
                    putchar('%');
                    break;
                default:
                    break;
            }
        } else {
            putchar(fmt[i]);
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
    vprintf_color(fmt, default_foreground_color, 0x000000, args);
    va_end(args);
}

void printInt(int num) {
    char buffer[40];
    uintToBase(num, buffer, 10);
    print_str(buffer);
}
