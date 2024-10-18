#ifndef TPE_COMMAND_H
#define TPE_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#include "../include/syscalls.h"

typedef struct {
    char * title;
    char * desc;
    Program command;
} Command;

void print_time();
void print_help();

void clear();
void divide(uint64_t argc, char *argv[]);
void invalidOpcode();
void changeSize(uint64_t argc, char *argv[]);
void play_music_cmd(uint64_t argc, char *argv[]);

void print_process_state();
void kill_process(uint64_t argc, char *argv[]);


int64_t test_processes_cmd(uint64_t argc, char *argv[]);
int64_t test_prio_cmd(uint64_t argc, char *argv[]);

// Getters de "Programs" (un Program es una función para inicializar procesos)
Program get_test_processes();
Program get_test_prio();

#endif //TPE_COMMAND_H