#ifndef TPE_ARQUI_KEYBOARD_H
#define TPE_ARQUI_KEYBOARD_H

#include <stdint.h>

typedef struct {
    int character;
    int make_code;
    int break_code;
    int specialFlag; // 0 for normally-accessed, 1 for shift-accessed, 2 for ctrl-accessed, 3 for alt-accessed
} AsciiMap;

extern char get_key();   // de libasm.asm
int map_key(char character, int flags[2]);
void keyboard_handler(uint64_t rsp);
int get_stdin();
int get_stdin_no_block();

#endif