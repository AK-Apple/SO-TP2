#ifndef TPE_SHELL_H
#define TPE_SHELL_H


#include <stdint.h>


void shell();
void printHeader();
void execute(char inputBuffer[]);
send_to_foreground(int pid);


#endif //TPE_SHELL_H