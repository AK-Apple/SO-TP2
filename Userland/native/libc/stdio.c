#include "../include/stdio.h"
#include <stdarg.h>
#include "../include/stdlib.h"

#define MAX_BUF 1024

void scanf(const char * fmt, ...) {
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
                    gets(va_arg(args, char), 1);
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

static void print_str(const char * str, uint64_t foreground, uint64_t background) {
    uint64_t i = 0;
    while (str[i]) {
        putchar(str[i++]);
    }
}

uint64_t printf_color(const char * fmt, uint64_t foreground, uint64_t background, ...) {
    va_list args;
    va_start(args, fmt);

    uint64_t i = 0;

    while (fmt[i]) {
        if (fmt[i] == '%') {
            char buf[MAX_BUF];
            switch (fmt[++i]) {
                case 'd':
                    itoa(va_arg(args, int), buf);
                    print_str(buf, foreground, background);
                    break;
                case 's':
                    print_str(buf, foreground, background);
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
        va_end(args);
    }
    return i;
}

void printf(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf_color(fmt, 0xFFFFFF, 0x000000, args);
    va_end(args);
}

void printInt(int num) {
    char buffer[40];
    uintToBase(num, buffer, 10);
    printf(buffer);
}

// void simplePrintf(char* str){
//    for (int i=0; str[i]; i++) putchar(str[i]);
// }