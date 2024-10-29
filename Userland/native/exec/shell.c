// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/shell.h"
#include "../include/syscalls.h"
#include "../include/command.h"
#include "../include/eliminator.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/sounds.h"
#include "../include/test_util.h"


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
    {"mem", "Imprime el estado de la memoria. Muestra la distribucion |size:bytes|size:free, despues stats", (Program)print_meminfo_cmd},
    {"fg", "manda un proceso a foreground", (Program)send_to_foreground, "<pid>"},
    {"block", "Cambia el estado de un proceso entre bloqueado y listo dado su PID.", (Program)block_cmd, "<pid>"},
    {"nice", "Cambia la prioridad de un proceso dado su PID y la nueva prioridad. prio:1:2:3:4:5", (Program)change_priority_cmd, "<pid> <prio>"},
};
static Process_Command processes[] = {
    {"testproc", "ejecuta test de proceso", get_test_processes, "<max_proc>"},
    {"testprio", "ejecuta test de prioridades", get_test_prio},
    {"testsync", "ejecuta test de sincronizacion. count=countdown, sem:0|1", get_test_sync, "<count> <sem>"},
    {"testmman", "ejecuta test de memoria. smart:0|1", get_test_mman, "<max> <smart>"},
    {"loop", "Imprime su ID con un saludo cada una determinada cantidad de segundos. msg es un mensaje opcional", get_endless_loop_print_seconds, "<secs_wait> <msg>"},
};
int foreground_pid = 0;
int shell_pid = 0;

uint64_t change_priority_cmd(uint64_t argc, char *argv[]) {
    if(argc < 3) {
        printf_error("wrong argcount.\nnice <pid> <prio>\n");
        return 1;
    }
    int64_t pid = satoi(argv[1]);
    int64_t priority = satoi(argv[2]);
    if(sys_get_process_status(pid) == 0) {
        printf_error("invalid pid %s\n", argv[1]);
        return 1;
    }
    if(priority < 0 || priority > 5) {
        printf_error("invalid priority %s\n", argv[2]);
        return 1;
    }
    sys_change_priority(pid, priority);

    return 0;
}

void print_meminfo_cmd() {
    Memory_Info info = {0};
    sys_memory_info(&info, MEM_COMPLETE);
}

uint64_t block_cmd(uint64_t argc, char *argv[]) {
    if(argc >= 2) {
        int64_t pid = atoi(argv[1]);
        if(pid == 0 || sys_get_process_status(pid) == 0) {
            printf_error("block invalid pid %s\n", argv[1]);
            return 1;
        }
        else if(sys_get_process_status(pid) == 3) {
            sys_unblock(pid);
            printf("unblocked process %d\n", pid);
        }  
        else {
            sys_block(pid);
            printf("blocked process %d\n", pid);
        }
    }
    else {
        printf_error("block recibe 1 argumento; <pid>\n");
    }
    return 0;
}

void print_help() {
    printf("Comandos built-in disponibles:\n");
    for (int i = 0 ; i < sizeof(commands)/sizeof(commands[0]) ; i++) {
        printf_color(commands[i].title, COLOR_ORANGE, 0);
        uint64_t total_len = strlen(commands[i].title);
        if(commands[i].args) {
            total_len += strlen(commands[i].args) + 1;
            printf_color(" %s", COLOR_YELLOW, 0, commands[i].args);
        }
        repeat_char(' ', 24 - total_len);
        printf(" : %s\n", commands[i].desc);
    }
    printf("Comandos que crean procesos:\n");
    printf_color("<command> &", COLOR_GREEN, 0);
    printf("              : escribir '&' al final del comando para ejecutarlo en background\n");
    for (int i = 0 ; i < sizeof(processes)/sizeof(processes[0]) ; i++) {
        printf_color(processes[i].title, COLOR_ORANGE, 0);
        uint64_t total_len = strlen(processes[i].title);
        if(processes[i].args) {
            total_len += strlen(processes[i].args) + 1;
            printf_color(" %s", COLOR_YELLOW, 0, processes[i].args);
        }
        repeat_char(' ', 24 - total_len);
        printf(" : %s\n", processes[i].desc);
    }
    printf("Hotkeys:\n");
    printf_color("ctrl c", COLOR_GREEN, 0);
    printf("   : matar al proceso en foreground y volver a la shell\n");
    printf_color("ctrl z", COLOR_GREEN, 0);
    printf("   : bloquear al proceso en foreground y volver a la shell\n");
    printf_color("ctrl x", COLOR_GREEN, 0);
    printf("   : mandar al proceso en foreground a background y volver a la shell\n");
    printf_color("ctrl d", COLOR_GREEN, 0);
    printf("   : para enviar EOF (end of file)\n");
    printf_color("left_alt", COLOR_GREEN, 0);
    printf(" : para guardar registros\n");
}

void send_to_foreground(int pid) {
    if(pid == 0) {
        printf_error("cant send init to foreground\n");
    }
    int process_state = sys_get_process_status(pid);
    if(process_state == 0) {
        foreground_pid = shell_pid;
    }
    else {
        sys_unblock(pid);
        foreground_pid = pid;
    }
}

void shell() {
    printf_color("Bienvenido a la Shell!! ", COLOR_GREEN, 0x000000);
    print_help();
    shell_pid = sys_get_pid(); 
    foreground_pid = shell_pid;

    do {
        if(foreground_pid == shell_pid) {
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
            if(foreground_pid != shell_pid && sys_get_process_status(foreground_pid) == 0) {
                foreground_pid = shell_pid;
                break;
            }
            if (hasRead) {
                if(foreground_pid != shell_pid) {
                    if(buf[0] == '\x3') {
                        printf_color("^C Process terminated pid=%d\n", COLOR_YELLOW, 0, foreground_pid);
                        sys_kill_process(foreground_pid);
                        foreground_pid = shell_pid;
                    }
                    else if(buf[0] == '\x4') { // TODO maybe ignore its stdout
                        printf_color("^Z Process blocked and sent to background pid=%d\n", COLOR_YELLOW, 0, foreground_pid);
                        sys_block(foreground_pid);
                        foreground_pid = shell_pid;
                    }
                    else if(buf[0] == '\x5') { // TODO maybe ignore its stdout 
                        printf_color("^X Process sent to run in background pid=%d\n", COLOR_YELLOW, 0, foreground_pid);
                        foreground_pid = shell_pid;
                    }
                    break;
                }
                if (!(buf[0]== 0x08 && i == 0))
                    sys_write(1, buf, 1);
                if (buf[0] == '\n') {
                    execute(command);
                    break_line = 1;
                } else if (buf[0] == 0x08 && i > 0) {
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
    compact_whitespace(inputBuffer);
    int argc = charcount(inputBuffer, ' ') + 1;
    char* argv[argc]; 
    int j = 0;
    argv[j++]=inputBuffer;
    for(int i = 0; inputBuffer[i]!=0; i++){
        if(inputBuffer[i]==' '){
            inputBuffer[i]=0;
            argv[j++]=&(inputBuffer[i+1]);
        }
    }
    int send_to_background = trim_end(argv[j-1], '&');
    if(argv[j-1][0] == '\0') argc--;
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
            const char *mode = "background";
            if(!send_to_background) {
                foreground_pid = pid;
                mode = "foreground";
            }
            printf_color("[shell] Running %s with pid=%d in %s...\n", COLOR_YELLOW, 0, argv[0], pid, mode);
            return;
        }
    }
    printf_error("Invalid command '%s', try 'help' command.\n", inputBuffer);
}