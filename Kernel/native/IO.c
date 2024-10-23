#include "IO.h"
#include "video.h"
#include <stdint.h>
#include <stdarg.h>
#include "font.h"
#include "lib.h"
#include "interrupts.h"


#define SIZE_BUFFER 1000

uint64_t x = 0;
uint64_t y = 0;
uint8_t fontSize = 1;

// el stdout no se guarda. Solo se guardan las coordenadas de la última posición

// el stdin es un array cíclico
static char stdinArr[SIZE_BUFFER];  
static int sizeIn = 0;
static int startsIn = 0;
static uint64_t global_foreground_color = 0x00FFFFFF;

static void print_str(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        putOut(str[i]);
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
                case 'd':
                    k_print_integer(va_arg(vars, int), padding, 10);
                    break;
                case 'x':
                    k_print_integer(va_arg(vars, int), padding, 16);
                    break;
                case 'b':
                    k_print_integer(va_arg(vars, int), padding, 2);
                    break;
                case 'c':
                    putOut(va_arg(vars, int));
                    break;
                case 's':
                    print_str(va_arg(vars, char*));
                    break;
                case '%':
                    putOut('%');
                    break;
                default:
                    break;
            }
        } else {
            putOut(fmt[i]);
        }
        i++;
    }
    return i;
}

int xOutOfBounds(uint64_t * x) {
    return *x + FONT_WIDTH * fontSize >= getWidth() || (int)*x < 0;     // casteo a int para que me tome que existen los negativos
}

int yOutOfBounds(uint64_t * y) {
    return *y + FONT_HEIGHT * fontSize >= getHeight() || (int)*y < 0;
}

void newLine(uint64_t * x, uint64_t * y) {
    *x = 0;
    *y += FONT_HEIGHT * fontSize;
}

// imprime caracter y modifica coordenadas. Usado para representar el stdout
void putCharAt(uint8_t c, uint64_t * x, uint64_t * y, uint64_t foreColor, uint64_t backgroundColor) {
    if (xOutOfBounds(x)) {
        newLine(x, y);
    }

    if (yOutOfBounds(y)) {
        clearScreen(backgroundColor);
        *y = 0;
    }

    uint8_t charMap[FONT_HEIGHT][FONT_WIDTH];
    get_char_map(c, charMap);

    for (int i = 0; i < FONT_HEIGHT ; i++) {
        for (int j = 0; j < FONT_WIDTH ; j++) {
            uint8_t squareIsOn = charMap[i][j];
            putSquare(squareIsOn ? foreColor : backgroundColor, *x + j * fontSize, *y + i * fontSize, fontSize);
        }
    }
    *x += FONT_WIDTH * fontSize;
}

void deleteCharAt(uint64_t * x, uint64_t * y, uint64_t backgroundColor) {
    if (*x == 0 && *y == 0) return;         // no borra si no hay nada
    *x -= FONT_WIDTH * fontSize;
    if (xOutOfBounds(x)) {
        int lastX = ((getWidth() -1) / (FONT_WIDTH * fontSize) - 1 ) * (FONT_WIDTH * fontSize);  // esto hay que ponerlo en otro lado
        *x = lastX;
        *y -= FONT_HEIGHT * fontSize;
    }
    putCharAt(' ', x, y, backgroundColor, backgroundColor);
    *x -= FONT_WIDTH * fontSize;
}

void putCharColoured(char c, uint64_t foreGround, uint64_t backGround) {
    switch (c) {
        case 0x0A:
            newLine(&x, &y);
            break;
        case 0x08:
            deleteCharAt(&x, &y, backGround);
            break;
        default:
            putCharAt(c, &x, &y, foreGround, backGround);
            break;
    }
}

void putChar(char c) {
    putCharColoured(c, 0xFFFFFF, BG_COLOR);
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
void putOut(char c){
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

void putIn(char c){
    // caso especial donde se pasa del límite: no se pueden agregar caracteres
    if (sizeIn >= SIZE_BUFFER-1) return;

    // mete c en el vector cíclico
    int pos = (startsIn + sizeIn) % SIZE_BUFFER;

    stdinArr[pos] = c;
    sizeIn++;
}

void clearIn(){
    sizeIn = 0;
}

// inspirado en la función de la API de Linux
void sys_write(int fd, const char* buf, int count){
    if (fd==1){
        for(int i=0; i<count; i++){
            putOut(buf[i]);
        }
    }
    if (fd==2){
        for(int i=0; i<count; i++){
            putErr(buf[i]);
        }
    }
}

// inspirado en la función de la API de Linux
int sys_read(int fd, char* buf, int count){
    int i=0;
    if (fd==0){
        for(i=0; i<count && i<sizeIn; i++){
            buf[i] = stdinArr[(startsIn + i)%SIZE_BUFFER];

        }
        startsIn+=i;
        startsIn = startsIn%SIZE_BUFFER;
        sizeIn-=i;
    }
    return i;
}


void sys_new_size(int newSize){
    _cli();               //por las dudas paro los in/out (antes generaba bugs)
    if (newSize < 1 || newSize > 5)
        return;
    fontSize = newSize;
    sys_clearScreen();
    _sti();
}

// resetea coordenadas
void sys_clearScreen(){
    clearScreen(BG_COLOR);
    y = 0;
    x = 0;
}