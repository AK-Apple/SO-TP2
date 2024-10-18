#include "../include/shell.h"
#include "../include/syscalls.h"
#include "../include/command.h"
#include "../include/eliminator.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/sounds.h"


#define MAX_BUF 1024

Command commands[] = {
        {"help", "Muestra la lista de comandos.", (Program) print_help},
        {"song", "pone musica con beeps. song <song_id> con song_id entre 1,2,3", play_music_cmd},
        {"time", "Muestra la hora.", print_time},
        {"eliminator", "Ejecuta el juego eliminator.", eliminator},
        {"size", "cambia tamanio de letra (entre 1 a 5).\t size <font_size>", changeSize},
        {"div", "divide num/den ; div <num> <den>", divide},
        {"invalidopcode", "Muestra excepcion de codigo invalido.", invalidOpcode},
        {"inforeg", "Muestra los registros guardados.", sys_getRegs},
        {"clear", "Limpia toda la pantalla.", clear},
        {"ps", "Lista la informacion de los procesos", print_process_state},
        {"kill", "mata un proceso dado un pid\t kill <pid>", kill_process},
        {"testp", "ejecuta test de proceso", test_processes_cmd},
        {"testprio", "ejecuta test de prioridades", test_prio_cmd},
        {"mem", "imprime la informacion de memoria dinamica", sys_print_mem},
};

void print_help() {
    printf("Presiona left alt para guardar registros en cualquier momento\n");
    printf("Comandos disponibles:\n");
    for (int i = 0 ; i < sizeof(commands)/sizeof(commands[0]) ; i++) {
        printf(commands[i].title);
        repeat_char(' ', 20 - strlen(commands[i].title));
        printf(" : ");
        printf(commands[i].desc);
        printf("\n");
    }
}

void shell() {
    printHeader();

    do {
        printf_color("user", COLOR_YELLOW, 0x000000);
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
                } else if (buf[0] != 0x08) {
                    command[i] = *buf;
                    i++;
                }
            }
        }
    } while (1);
}
void printHeader() {
    printf_color("Bienvenido a la Shell!\n", 0xcdff00, 0x000000);
    print_help();
}

void execute(char inputBuffer[]) {
    int command_count = sizeof(commands) / sizeof(commands[0]);
    int argc = charcount(inputBuffer, ' ') + 1;

    char* argv[argc]; //Seguro entra en el stack, el tamaño es <= B_SIZE size, y el stack esde 4MiB.
    int j = 0;
    argv[j++]=inputBuffer;
    for(int i = 0; inputBuffer[i]!=0; i++){
        if(inputBuffer[i]==' '){
            inputBuffer[i]=0;
            argv[j++]=&(inputBuffer[i+1]);
        }
    }
    for (int i = 0; i < command_count ; i++)
    {
        if (strcmp(argv[0], commands[i].title) == 0)
        {
            commands[i].command(argc, argv);
            return;
        }
    }
    printf_error("Invalid command, try again.\n");
}