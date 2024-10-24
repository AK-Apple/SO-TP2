#ifndef TPE_COMMAND_H
#define TPE_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#include "../include/syscalls.h"

typedef struct {
    char * title;
    char * desc;
    Program command;
    char * args;
} Command;
typedef struct {
    char * title;
    char * desc;
    Program (*process_getter)();
    char * args;
} Process_Command;

void print_time();
void print_help();

void divide(uint64_t argc, char *argv[]);
void invalidOpcode();
void changeSize(uint64_t argc, char *argv[]);
void play_music_cmd(uint64_t argc, char *argv[]);

void kill_process(uint64_t argc, char *argv[]);

// Getters de "Programs" (un Program es una función para inicializar procesos)
Program get_test_processes();
Program get_test_prio();
Program get_test_sync();
Program get_test_mman();
Program get_endless_loop_print_seconds();

#endif //TPE_COMMAND_H