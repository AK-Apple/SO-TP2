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
    {"help", "Muestra la lista de comandos.", (Program)print_help},
    {"song", "pone musica con beeps. Con song_id:1|2|3", (Program)play_music_cmd, "<song_id>"},
    {"time", "Muestra la hora.", (Program)print_time},
    {"eliminator", "Ejecuta el juego eliminator.", (Program)eliminator},
    {"size", "cambia tamanio de letra (entre 1 a 5).", (Program)changeSize, "<font_size>"},
    {"div", "divide num/den", (Program)divide, "<num> <den>"},
    {"invalidopcode", "Muestra excepcion de codigo invalido.", (Program)invalidOpcode},
    {"inforeg", "Muestra los registros guardados. (Presiona `left_alt` para guardar registros)", (Program)sys_getRegs},
    {"clear", "Limpia toda la pantalla.", (Program)sys_clear},
    {"ps", "Lista la informacion de los procesos", (Program)sys_print_all_processes},
    {"kill", "mata un proceso dado un pid", (Program)kill_process, "<pid>"},
    {"mem", "imprime la informacion de memoria dinamica", (Program)sys_memory_info},
    {"fg", "manda un proceso a foreground", (Program)send_to_foreground, "<pid>"},
    {"block", "bloquea un proceso dado un pid", (Program)block_cmd, "<pid>"},
    {"unblock", "desbloquea un proceso dado un pid", (Program)unblock_cmd, "<pid>"},
};
static Process_Command processes[] = {
    {"testproc", "ejecuta test de proceso", get_test_processes, "<max_proc>"},
    {"testprio", "ejecuta test de prioridades", get_test_prio},
    {"testsync", "ejecuta test de sincronizacion. count=countdown, sem:0|1", get_test_sync, "<count> <sem>"},
    {"testmman", "ejecuta test de memoria. smart:0|1", get_test_mman, "<max> <smart>"},
};
int active_pid = 0;
int shell_pid = 0;

uint64_t block_cmd(uint64_t argc, char *argv[]) {
    if(argc >= 2) {
        int64_t pid = atoi(argv[1]);
        if(pid > 0 && sys_get_process_status(pid) != 0) {
            sys_block(pid);
        }  
        else {
            printf_error("block invalid argument %s\n", argv[1]);
        }
    }
    else {
        printf_error("block recibe 1 argumento; <pid>\n");
    }
    return 0;
}

uint64_t unblock_cmd(uint64_t argc, char *argv[]) {
    if(argc >= 2) {
        int64_t pid = atoi(argv[1]);
        if(pid > 0 && sys_get_process_status(pid) != 0) {
            sys_unblock(pid);
        }  
        else {
            printf_error("unblock invalid pid %s\n", argv[1]);
        }
    }
    else {
        printf_error("unblock recibe 1 argumento; <pid>\n");
    }
    return 0;
}

void print_help() {
    printf("Comandos built-in disponibles:\n");
    for (int i = 0 ; i < sizeof(commands)/sizeof(commands[0]) ; i++) {
        printf_color(commands[i].title, COLOR_GREEN, 0);
        uint64_t total_len = strlen(commands[i].title);
        if(commands[i].args) {
            total_len += strlen(commands[i].args) + 1;
            printf_color(" %s", COLOR_YELLOW, 0, commands[i].args);
        }
        repeat_char(' ', 24 - total_len);
        printf(" : %s\n", commands[i].desc);
    }
    printf("Comandos que crean procesos:\n");
    for (int i = 0 ; i < sizeof(processes)/sizeof(processes[0]) ; i++) {
        printf_color(processes[i].title, COLOR_GREEN, 0);
        uint64_t total_len = strlen(processes[i].title);
        if(processes[i].args) {
            total_len += strlen(processes[i].args) + 1;
            printf_color(" %s", COLOR_YELLOW, 0, processes[i].args);
        }
        repeat_char(' ', 24 - total_len);
        printf(" : %s\n", processes[i].desc);
    }
    printf("Hotkeys:\n");
    printf("ctrl c   : para matar al proceso activo y volver a la shell\n");
    printf("ctrl z   : para bloquar al proceso activo y volver a la shell\n");
    printf("ctrl x   : para seguir corriendo el proceso activo y volver a la shell\n");
    printf("ctrl d   : para mandar EOF\n");
    printf("left_alt : para guardar registros\n");
}

void send_to_foreground(int pid) {
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
    printf_color("Bienvenido a la Shell!! ", COLOR_GREEN, 0x000000);
    print_help();
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
    for (int i = 0; i < sizeof(processes)/sizeof(processes[0]) ; i++)
    {
        if (strcmp(argv[0], processes[i].title) == 0)
        {
            int pid = sys_create_process(processes[i].process_getter(), argc, argv);
            printf("[shell] Running %s with pid %d...\n", argv[0], pid);
            active_pid = pid;
            return;
        }
    }
    printf_error("Invalid command, try again.\n");
}