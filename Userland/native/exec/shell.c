#include "../include/shell.h"
#include "../include/syscalls.h"
#include "../include/command.h"
#include "../include/eliminator.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/sounds.h"


#define MAX_BUF 1024

static Command commands[] = {
        {"help", "Muestra la lista de comandos.", (Program) print_help},
        {"song", "pone musica con beeps. song <song_id> con song_id entre 1,2,3", play_music_cmd},
        {"time", "Muestra la hora.", print_time},
        {"eliminator", "Ejecuta el juego eliminator.", eliminator},
        {"size", "cambia tamanio de letra (entre 1 a 5).\t size <font_size>", changeSize},
        {"div", "divide num/den ; div <num> <den>", divide},
        {"invalidopcode", "Muestra excepcion de codigo invalido.", invalidOpcode},
        {"inforeg", "Muestra los registros guardados. (Presiona `left_alt` para guardar registros en cualquier momento)", sys_getRegs},
        {"clear", "Limpia toda la pantalla.", clear},
        {"ps", "Lista la informacion de los procesos", print_process_state},
        {"kill", "mata un proceso dado un pid\t kill <pid>", kill_process},
        {"testp", "ejecuta test de proceso", test_processes_cmd},
        {"testprio", "ejecuta test de prioridades", test_prio_cmd},
        {"mem", "imprime la informacion de memoria dinamica", sys_print_mem},
        {"fg", "manda un proceso a foreground\t fd <pid>", send_to_foreground},
};
int active_pid = 0;
int shell_pid = 0;

void print_help() {
    printf("Comandos disponibles:\n");
    for (int i = 0 ; i < sizeof(commands)/sizeof(commands[0]) ; i++) {
        printf(commands[i].title);
        repeat_char(' ', 20 - strlen(commands[i].title));
        printf(" : ");
        printf(commands[i].desc);
        printf("\n");
    }
}

send_to_foreground(int pid) {
    if(pid == 0) {
        printf_error("cant send init to foreground\n");
    }
    int process_state = sys_get_process_status(pid);
    if(process_state == 0) {
        active_pid = shell_pid;
    }
    else {
        sys_unblock(pid);
        active_pid = pid;
    }
}

void shell() {
    printHeader();
    shell_pid = sys_get_pid(); 
    active_pid = shell_pid;

    do {
        if(active_pid == shell_pid) {
            printf_color("user@grupo20", COLOR_GREEN, 0x000000);
            putchar(':');
            printf_color("~", COLOR_BLUE, 0);
            printf("$ ");
        }
        int break_line = 0;
        int i = 0;
        char command[MAX_BUF] = {0};
        while (!break_line) {
            char buf[1];
            char hasRead = sys_read(0, buf, 1);
            if(active_pid != shell_pid && sys_get_process_status(active_pid) == 0) {
                active_pid = shell_pid;
                break;
            }
            if (hasRead) {
                if(active_pid != shell_pid) {
                    if(buf[0] == '\x3') {
                        printf_color("^C Process terminated pid=%d\n", COLOR_YELLOW, 0, active_pid);
                        sys_kill_process(active_pid);
                        active_pid = shell_pid;
                    }
                    else if(buf[0] == '\x4') { // TODO maybe ignore its stdout
                        printf_color("^Z Process blocked and sent to background pid=%d\n", COLOR_YELLOW, 0, active_pid);
                        sys_block(active_pid);
                        active_pid = shell_pid;
                    }
                    else if(buf[0] == '\x5') { // TODO maybe ignore its stdout 
                        printf_color("^X Process sent to run in background pid=%d\n", COLOR_YELLOW, 0, active_pid);
                        active_pid = shell_pid;
                    }
                    break;
                }
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
    printf_color("Bienvenido a la Shell!! ", COLOR_GREEN, 0x000000);
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