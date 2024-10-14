#ifndef TPE_COMMAND_H
#define TPE_COMMAND_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    char * title;
    char * desc;
    void (*command)();
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
void kill_process(int pid);
int64_t test_processes_cmd(uint64_t argc, char *argv[]);
void test_prio_cmd();

void test_prio();
int64_t test_processes(uint64_t argc, char *argv[]);

//para excepciones (buscan en Assembler)
extern void runInvalidOpcodeException(void);
extern void runZeroDivisionException(void);

#endif //TPE_COMMAND_H