#include "../include/shell.h"
#include "../include/syscalls.h"
#include "../include/command.h"
#include "../include/eliminator.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/sounds.h"


#define MAX_BUF 1024

// se podría cambiar la implementación para que se acepten parámetros (y no haya que hacer size 1, size 2...)
// pero en este caso no hace falta porque hay pocos parámetros
char * commands[] = {"clear", "dividebyzero", "help", "inforeg", "invalidopcode", "time", "eliminator",
                     "size_1", "size_2", "size_3", "size_4", "size_5"};
void (*commands_functions[])() = {clear,  divideByZero, print_help, sys_getRegs, invalidOpcode, time, eliminator,
                                  changeSize_1, changeSize_2, changeSize_3, changeSize_4, changeSize_5};

void shell() {
    play_song(2);
    while (next_part());
    printHeader();

    do {
        printf_color("user", 0xcdff00, 0x000000);
        printf(":~$ ");
        int break_line = 0;
        int i = 0;
        char command[MAX_BUF] = {0};
        while (!break_line) {
            char buf[1];
            char hasRead = sys_read(0, buf, 1);
            if (hasRead) {
                if (!(buf[0]== 0x08 && i == 0))
                    sys_write(1, buf, 1);
                if (buf[0] == '\n') {
                    execute(command);
                    break_line = 1;
                } else if (buf[0] == 0x08 && i > 0) {  // borrado (tiene que haber algo en el buffer)
                    command[i - 1] = 0;
                    i--;
                } else if (buf[0] != 0x08) {        //
                    command[i] = *buf;
                    i++;
                }
            }
        }
    } while (1);
}
void printHeader() {
    printf_color("Bienvenido a la Shell!\n", 0xcdff00, 0x000000);
    printf("Ejecuta 'help' para ver una lista de comandos.\n");
    return;
}

void execute(const char *inputBuffer) {
    for (int i = 0; i < 12 ; i++)
    {
        if (strcmp(inputBuffer, commands[i]) == 0)
        {
            commands_functions[i]();
            return;
        }
    }
    printf("Invalid command, try again.\n");
}