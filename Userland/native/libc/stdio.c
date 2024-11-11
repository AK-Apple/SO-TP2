// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdio.h"
#include "shared.h"
#include <stdarg.h>
#include "stdlib.h"
#include "syscalls.h"
#include "string.h"

#define MAX_BUF 1024

static uint64_t default_foreground_color = 0x00FFFFFF;

static uint64_t vfprintf_color(int fd, char *fmt, uint64_t foreground, uint64_t background, va_list vars);

void putchar(char c) 
{
    char buf[] = {c, 0};
    sys_write(STDOUT, buf, 1);
}

void fputchar(char c, int fd) 
{
    char buf[] = {c, 0};
    sys_write(fd, buf, 1);
}

void putcharColoured(char c, uint64_t foreground, uint64_t background) 
{
    if(foreground != default_foreground_color) 
    {
        sys_set_text_color(foreground);
    }
    
    putchar(c);

    if(foreground != default_foreground_color) 
    {
        sys_set_text_color(default_foreground_color);
    }
}

void repeat_char(int fd, char c, int count) 
{
    char buf[count];
    memset(buf, c, count);
    sys_write(fd, buf, count);
}

void scanf(char * fmt, ...) 
{
    va_list args;
    va_start(args, fmt);

    char buf[MAX_BUF] = {0};

    for (int i = 0; fmt[i] != '\0' ; i++) 
    {
        if (fmt[i++] == '%') 
        {
            switch (fmt[i]) 
            {
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

uint64_t printf_color(char * fmt, uint64_t foreground, uint64_t background, ...) 
{
    va_list args;
    va_start(args, background);  

    uint64_t i = vfprintf_color(STDOUT, fmt, foreground, background, args);

    va_end(args);  
    return i;
}

uint64_t fprintf_color(int fd, char * fmt, uint64_t foreground, uint64_t background, ...) 
{
    va_list args;
    va_start(args, background);  

    uint64_t i = vfprintf_color(fd, fmt, foreground, background, args);

    va_end(args);  
    return i;
}

void printf_error(char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);
    vfprintf_color(STDERR, fmt, default_foreground_color, 0x000000, args); 
    va_end(args);
}

static uint64_t vfprintf_color(int fd, char *fmt, uint64_t foreground, uint64_t background, va_list vars) 
{
    uint64_t i = 0;

    if(foreground != default_foreground_color) 
    {
        sys_set_text_color(foreground);
    }

    while (fmt[i]) 
    {
        if (fmt[i] == '%') 
        {
            char buf[MAX_BUF] = {0};
            i++;
            int64_t padding = atoi_index(fmt, &i);
            padding = (padding == 0) ? 1 : padding;
            int64_t len = 0;
            switch (fmt[i]) 
            {
                case 'l':
                    i++;
                    switch (fmt[i])
                    {
                    case 'u':
                        len = uint_to_base(va_arg(vars, uint64_t), buf, 10);
                        break;
                    case 'd':
                        len =  (uint64_t)(itoa(va_arg(vars, int64_t), buf, 10) - buf);
                        break;
                    case 'x':
                        len = uint_to_base(va_arg(vars, uint64_t), buf, 16);
                        break;
                    default:
                        break;
                    }                   
                    break;
                case 'u':
                    len = uint_to_base(va_arg(vars, uint32_t), buf, 10);
                    break;
                case 'd':
                    len = (uint64_t)(itoa(va_arg(vars, int32_t), buf, 10) - buf);
                    break;
                case 'x':
                    len = uint_to_base(va_arg(vars, int), buf, 16);
                    break;
                case 'b':
                    len = uint_to_base(va_arg(vars, int), buf, 2);
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
            if(padding - len > 0) 
            {
                repeat_char(fd, '0', padding - len);
            }
            sys_write(fd, buf, len);
            i++;
        } else 
        {
            uint64_t j = i;
            while(fmt[j] && fmt[j] != '%') j++;
            sys_write(fd, &fmt[i], j - i);
            i = j;
        }
    }

    if(foreground != default_foreground_color) 
    {
        sys_set_text_color(default_foreground_color);
    }
    return i;
}

void printf(char * fmt, ...) 
{
    va_list args;
    va_start(args, fmt);
    vfprintf_color(STDOUT, fmt, default_foreground_color, 0x000000, args);
    va_end(args);
}

void fprinf(int fd, char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);
    vfprintf_color(fd, fmt, default_foreground_color, 0x000000, args);
    va_end(args);
}
