#ifndef TPE_COMMAND_H
#define TPE_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#include "../include/syscalls.h"

typedef struct {
    char * title;
    char * desc;
    Program command;
} command;

void print_time();
void print_help();

void clear();
void divideByZero();
void invalidOpcode();
void time();
void changeSize_1();
void changeSize_2();
void changeSize_3();
void changeSize_4();
void changeSize_5();

void print_process_state();
void kill_process(uint64_t argc, char *argv[]);


int64_t test_processes_cmd(uint64_t argc, char *argv[]);
int64_t test_prio_cmd(uint64_t argc, char *argv[]);

// Getters de "Programs" (un Program es una función para inicializar procesos)
Program get_test_processes();
Program get_test_prio();

//para excepciones (buscan en Assembler)
extern void runInvalidOpcodeException(void);
extern void runZeroDivisionException(void);

#endif //TPE_COMMAND_H