#ifndef TPE_SHELL_H
#define TPE_SHELL_H


#include <stdint.h>


void shell();
void execute(char inputBuffer[]);
int64_t send_to_foreground(uint64_t argc, char *argv[]);
uint64_t block_cmd(uint64_t argc, char *argv[]);
uint64_t change_priority_cmd(uint64_t argc, char *argv[]);
void print_meminfo_cmd();

#endif //TPE_SHELL_H