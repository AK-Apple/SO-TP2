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
int64_t inforeg(uint64_t argc, char *argv[]);
void print_processes_state();
uint64_t block_cmd(uint64_t argc, char *argv[]);
uint64_t change_priority_cmd(uint64_t argc, char *argv[]);
void print_meminfo_cmd();

void divide(uint64_t argc, char *argv[]);
void invalid_opcode();
void change_size(uint64_t argc, char *argv[]);

void play_music_cmd(uint64_t argc, char *argv[]);
void play_infinite_music_cmd(uint64_t argc, char *argv[]);

void kill_process(uint64_t argc, char *argv[]);

int wc(int argc, char *argv[]);
int cat(int argc, char *argv[]);
int filter(int argc, char *argv[]);
void userland_sleep(uint64_t seconds);

void mvar_test();
//  Funciones de tipo "Program" (es decir, una función con la que se inicializa un proceso)

int64_t endless_loop_print_seconds(uint64_t argc, char* argv[]);
int64_t test_processes(uint64_t argc, char *argv[]);
int64_t test_prio(uint64_t argc, char *argv[]);
int64_t test_sync(uint64_t argc, char *argv[]);
int64_t test_mm(uint64_t argc, char *argv[]);
int64_t test_pipe(uint64_t argc, char *argv[]);
int64_t phylo(uint64_t argc, char *argv[]);
int64_t echo_cmd(uint64_t argc, char *argv[]);


#endif //TPE_COMMAND_H