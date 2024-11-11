#ifndef TPE_SHELL_H
#define TPE_SHELL_H


#include <stdint.h>


void shell();
void execute(char inputBuffer[]);
int64_t send_to_foreground(uint64_t argc, char *argv[]);

#endif //TPE_SHELL_H