// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/command.h"
#include "../include/syscalls.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/eliminator.h"
#include "../include/test_util.h"
#include "../include/sounds.h"
#include "../include/string.h"

#define REG_SIZE 17
#define SECONDS 0
#define MINUTES 2
#define HOURS 4
#define DAY_OF_THE_WEEK 6
#define DAY_OF_THE_MONTH 7
#define MONTH 8
#define YEAR 9

int wc(int argc, char *argv[]) {
    int input_char = 0;
    int line_count = 0;
    while((input_char = getchar()) != EOF) {
        line_count += (input_char == '\n');
    }
    printf("cantidad de lineas = %d\n", line_count);

    return 0;
}

int cat(int argc, char *argv[]) {
    int input_char = 0;
    while((input_char = getchar()) != EOF) {
        putchar(input_char);
    }
    return 0;
}

int filter(int argc, char *argv[]) {
    int input_char = 0;
    while((input_char = getchar()) != EOF) {
        if(!is_vowel(input_char))
            putchar(input_char);
    }
    return 0;
}

int64_t echo_cmd(uint64_t argc, char *argv[]) {
    for(int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}

void play_music_cmd(uint64_t argc, char *argv[]) {
    uint64_t song_id = 1;
    if(argc >= 2) {
        song_id = satoi(argv[1]);
        if(song_id < 1 || song_id > 3) {
            song_id = 1;
        }
    }
    play_song(song_id);
    while (next_part()); // esto bloquea la consola, quiza en un futura hacer que corra en background
}

void divide(uint64_t argc, char *argv[]) {
    if(argc == 3) {
        int64_t a, b;
        a = satoi(argv[1]);    
        b = satoi(argv[2]);
        int64_t res = a / b;
        printf("%d / %d = %d\n", a, b, res);    
    }
    else {
        printf_error("se recibio: '%s' '%s'\nusage: div <a> <b>\n", argv[1], argv[2]);
    }
}

void invalidOpcode() {
    printf("executing invalid opcode...\n");
    static uint8_t invalidOpcodes[] = {0x06, 0x07};
    ((void (*)()) invalidOpcodes)();
}

void changeSize(uint64_t argc, char *argv[]) {
    uint32_t new_size = 1;
    if (argc >= 2) {
        new_size = satoi(argv[1]);
        if(new_size < 1 || new_size > 5) {
            new_size = 1;
        }
    }
    sys_new_size(new_size);
}

void kill_process(uint64_t argc, char *argv[]) {
    int pid = satoi(argv[1]);
    printf("[killing process] pid=%d\n", pid);
    if(argc != 2) {
        printf_error("invalid arguments\n"); 
        return;
    }
    if(pid < 1) {
        printf_error("invalid pid\n");
        return;
    }
    sys_kill_process(pid);
}

void print_time() {
    int hours = sys_time(HOURS);
    printInt(hours);
    printf(":");
    int minutes = sys_time(MINUTES);
    if(minutes < 10)
        printf("0");
    printInt(minutes);
    printf(", ");
    printInt(sys_time(DAY_OF_THE_MONTH));
    printf("/");
    printInt(sys_time(MONTH));
    printf("/");
    printInt(sys_time(YEAR) + 2000);
    printf("\n");
}