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

void print_time();
void print_help();

void divide(uint64_t argc, char *argv[]);
void invalidOpcode();
void changeSize(uint64_t argc, char *argv[]);
void play_music_cmd(uint64_t argc, char *argv[]);

void kill_process(uint64_t argc, char *argv[]);

int wc(int argc, char *argv[]);
int cat(int argc, char *argv[]);
int filter(int argc, char *argv[]);

// Getters de "Programs" (un Program es una función para inicializar procesos)
int64_t endless_loop_print_seconds(uint64_t argc, char* argv[]);

int64_t test_processes(uint64_t argc, char *argv[]);
void test_prio();
int64_t test_sync(uint64_t argc, char *argv[]);
int64_t test_mm(uint64_t argc, char *argv[]);
int64_t test_pipe(uint64_t argc, char *argv[]);
int64_t phylo(uint64_t argc, char *argv[]);

#endif //TPE_COMMAND_H