// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "IO.h"
#include "video.h"
#include <stdint.h>
#include <stdarg.h>
#include "font.h"
#include "lib.h"
#include "interrupts.h"
#include "keyboard.h"
#include "pipes.h"
#include "process.h"

uint64_t screen_x = 0;
uint64_t screen_y = 0;
uint8_t font_size = 1;
static uint64_t global_foreground_color = 0x00FFFFFF;




int sys_read(int fd_index, char* buf, int count)
{
    int (*stdin_getter)() = get_stdin_options() == BLOCK_ENABLED ? get_stdin : get_stdin_no_block;

    int fd = get_fd(fd_index);
    int i = 0;
    if (fd == STDIN)
    {
        for (i = 0; i<count; i++)
        {
            buf[i] = stdin_getter();
            if ((int) buf[i] == -1)
				return i + 1;
        }
        return i;
    }
    else if (fd > 2)
    {
        if (get_stdin_options() == BLOCK_DISABLED)
        {
            printf_error("BLOCK_DISABLED not implemented on pipes");
            return 0;
        }
        return read_pipe(fd, buf, count);
    }
    else if(fd == DEVNULL) {
        buf[0] = EOF;
        buf[1] = '\0';
    }
    return 0;
}


static void print_str(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        put_out(str[i]);
    }
}

static uint64_t vprintf(char *fmt, va_list vars) {
    uint64_t i = 0;

    while (fmt[i]) {
        if (fmt[i] == '%') {
            i++;
            uint64_t padding = k_atoi_index(fmt, &i);
            padding = (padding == 0) ? 1 : padding;
            switch (fmt[i]) {
                case 'l':
                    i++;
                    switch (fmt[i])
                    {
                    case 'd':
                    case 'u':
                        k_print_integer(va_arg(vars, uint64_t), padding, 10);
                        break;
                    case 'x':
                        k_print_integer(va_arg(vars, uint64_t), padding, 16);
                        break;
                    default:
                        break;
                    }
                    break;
                case 'd':
                    k_print_integer(va_arg(vars, int), padding, 10);
                    break;
                case 'x':
                    k_print_integer(va_arg(vars, int), padding, 16);
                    break;
                case 'b':
                    k_print_integer(va_arg(vars, uint64_t), padding, 2);
                    break;
                case 'c':
                    put_out(va_arg(vars, int));
                    break;
                case 's':
                    print_str(va_arg(vars, char*));
                    break;
                case '%':
                    put_out('%');
                    break;
                default:
                    break;
            }
        } else {
            put_out(fmt[i]);
        }
        i++;
    }
    return i;
}

int xOutOfBounds(uint64_t * x) {
    return *x + FONT_WIDTH * font_size >= get_width() || (int)*x < 0;     // casteo a int para que me tome que existen los negativos
}

int yOutOfBounds(uint64_t * y) {
    return *y + FONT_HEIGHT * font_size >= get_height() || (int)*y < 0;
}

void newLine(uint64_t * x, uint64_t * y) {
    *x = 0;
    *y += FONT_HEIGHT * font_size;
}

// imprime caracter y modifica coordenadas. Usado para representar el stdout
void putCharAt(uint8_t c, uint64_t * x, uint64_t * y, uint64_t foreColor, uint64_t backgroundColor) {
    if (xOutOfBounds(x)) {
        newLine(x, y);
    }

    if (yOutOfBounds(y)) {
        clear_screen(backgroundColor);
        *y = 0;
    }

    uint8_t charMap[FONT_HEIGHT][FONT_WIDTH];
    get_char_map(c, charMap);

    for (int i = 0; i < FONT_HEIGHT ; i++) {
        for (int j = 0; j < FONT_WIDTH ; j++) {
            uint8_t squareIsOn = charMap[i][j];
            put_square(squareIsOn ? foreColor : backgroundColor, *x + j * font_size, *y + i * font_size, font_size);
        }
    }
    *x += FONT_WIDTH * font_size;
}

void deleteCharAt(uint64_t * x, uint64_t * y, uint64_t backgroundColor) {
    if (*x == 0 && *y == 0) return;         // no borra si no hay nada
    *x -= FONT_WIDTH * font_size;
    if (xOutOfBounds(x)) {
        int lastX = ((get_width() -1) / (FONT_WIDTH * font_size) - 1 ) * (FONT_WIDTH * font_size);  // esto hay que ponerlo en otro lado
        *x = lastX;
        *y -= FONT_HEIGHT * font_size;
    }
    putCharAt(' ', x, y, backgroundColor, backgroundColor);
    *x -= FONT_WIDTH * font_size;
}

void putCharColoured(char c, uint64_t foreGround, uint64_t backGround) {
    switch (c) {
        case 0x0A:
            newLine(&screen_x, &screen_y);
            break;
        case 0x08:
            deleteCharAt(&screen_x, &screen_y, backGround);
            break;
        case EOF:
            break;
        default:
            putCharAt(c, &screen_x, &screen_y, foreGround, backGround);
            break;
    }
}

void printf(char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void printf_color(char * fmt, uint64_t foreground, ...) {
    va_list args;
    va_start(args, foreground);

    uint64_t previous_foreground = global_foreground_color;
    set_foreground_color(foreground);
    vprintf(fmt, args);
    set_foreground_color(previous_foreground);

    va_end(args);
}

void printf_error(char * fmt, ...) {
    va_list args;
    va_start(args, fmt);

    uint64_t previous_foreground = global_foreground_color;
    set_foreground_color(0x00FF0000);
    vprintf(fmt, args);
    set_foreground_color(previous_foreground);

    va_end(args);
}

// Funciones que manejan stdin, stdout y stderr
void put_out(char c){
    putCharColoured(c, global_foreground_color, BG_COLOR);
}

void set_foreground_color(uint64_t fg) {
    if(fg > 0x00FFFFFF) {
        fg = 0x00FFFFFF;
    }
    global_foreground_color = fg;
}

void putErr(char c){
    putCharColoured(c, 0x00ff0000, BG_COLOR);
}

// inspirado en la función de la API de Linux
void sys_write(int fd_index, const char* buf, int count)
{
    int fd = get_fd(fd_index);
    if (fd==1)
    {
        for(int i = 0; i<count; i++)
        {
            put_out(buf[i]);
        }
    }
    if (fd==2)
    {
        for(int i = 0; i<count; i++)
        {
            putErr(buf[i]);
        }
    }
    if (fd > 2)
    {
        write_pipe(fd, buf, count);
    }
    return;
}


void sys_new_size(int newSize){
    _cli();               //por las dudas paro los in/out (antes generaba bugs)
    if (newSize < 1 || newSize > 5)
        return;
    font_size = newSize;
    sys_clear_screen();
    _sti();
}

// resetea coordenadas
void sys_clear_screen(){
    clear_screen(BG_COLOR);
    screen_y = 0;
    screen_x = 0;
}