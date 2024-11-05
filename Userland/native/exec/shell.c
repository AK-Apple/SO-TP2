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
#include "shared.h"


#define MAX_BUF 1024

static Command commands[] = {
    {"testproc", "ejecuta test de proceso", (Program)test_processes, "<max_proc>"},
    {"testprio", "ejecuta test de prioridades", (Program)test_prio},
    {"testsync", "ejecuta test de sincronizacion. count=countdown, sem:0|1", (Program)test_sync, "<count> <sem>"},
    {"testmman", "ejecuta test de memoria. smart:0|1", (Program)test_mm, "<max> <smart>"},
    {"testpipe", "ejecuta test de named pipes", (Program)test_pipe},
    {"loop", "Imprime su ID con un saludo cada una determinada cantidad de segundos. msg es un mensaje opcional", (Program)endless_loop_print_seconds, "<secs_wait> <msg>"},
    {"wc", "cuenta la cantidad de lineas del stdin terminando con EOF(Ctrl+D)", (Program)wc},
    {"cat", "printea el input", (Program)cat},
    {"filter", "Filtra las vocales del input", (Program)filter},
    {"phylo", "ejecuta programa de phylo", (Program)phylo},
    {"echo", "imprime en stdout los argumentos que le pasas", (Program)echo_cmd, "<args...>"},
    {"div", "divide num/den", (Program)divide, "<num> <den>"},
    {"invalidopcode", "Muestra excepcion de codigo invalido.", (Program)invalidOpcode},
    {"help", "Muestra la lista de comandos.", (Program)print_help},
    {"song", "pone musica con beeps. Con song_id:1|2|3", (Program)play_music_cmd, "<song_id>"},
    {"time", "Muestra la hora.", (Program)print_time},
    {"eliminator", "Ejecuta el juego eliminator.", (Program)eliminator},
    {"size", "cambia tamanio de letra (entre 1 a 5).", (Program)changeSize, "<font_size>"},
    {"inforeg", "Muestra los registros guardados. (Presiona `left_alt` para guardar registros)", (Program)sys_getRegs},
    {"clear", "Limpia toda la pantalla.", (Program)sys_clear},
    {"ps", "Lista la informacion de los procesos", (Program)sys_print_all_processes},
    {"kill", "mata un proceso dado un pid", (Program)kill_process, "<pid>"},
    {"mem", "Imprime el estado de la memoria. Muestra la distribucion |size:bytes|size:free, despues stats", (Program)print_meminfo_cmd},
    {"fg", "manda un proceso a foreground", (Program)send_to_foreground, "<pid>"},
    {"block", "Cambia el estado de un proceso entre bloqueado y listo dado su PID.", (Program)block_cmd, "<pid>"},
    {"nice", "Cambia la prioridad de un proceso dado su PID y la nueva prioridad. prio:0=low ; 1=mid ; 2=high", (Program)change_priority_cmd, "<pid> <prio>"},
};

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
    if(priority > PRIORITY_HIGH) {
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
            printf("unblocked process %ld\n", pid);
        }  
        else {
            sys_block(pid);
            printf("blocked process %ld\n", pid);
        }
    }
    else {
        printf_error("block recibe 1 argumento; <pid>\n");
    }
    return 0;
}

void print_help() {
    sys_clear();
    printf("Comandos disponibles:\n");
    printf_color("<command> &", COLOR_GREEN, 0);
    printf("              : escribir '&' al final del comando para ejecutarlo en background\n");
    printf_color("<command1> | <command2>", COLOR_GREEN, 0);
    printf("  : escribir dos comandos separados por '|'  para pipear cmd1 a cmd2 \n");
    for (int i = 0 ; i < sizeof(commands)/sizeof(commands[0]) ; i++) {
        printf_color(commands[i].title, COLOR_ORANGE, 0);
        uint64_t total_len = strlen(commands[i].title);
        if(commands[i].args) {
            total_len += strlen(commands[i].args) + 1;
            printf_color(" %s", COLOR_YELLOW, 0, commands[i].args);
        }
        repeat_char(STD_OUT, ' ', 24 - total_len);
        printf(" : %s\n", commands[i].desc);
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
    printf_color("ctrl a", COLOR_GREEN, 0);
    printf("   : para escribir el caracter '&' (es la alternativa a shift+6)\n");
    printf_color("ctrl p", COLOR_GREEN, 0);
    printf("   : para escribir el caracter '|' (es la alternativa a escribir '|' en teclado castellano)\n");
}

int64_t send_to_foreground(uint64_t argc, char *argv[]) {
    if(argc < 1) {
        printf_error("usage: fg <pid>\n");
        return -1;
    }
    int pid = atoi(argv[1]);
    if(pid == 0) {
        printf_error("cant send init to foreground\n");
        return -1;
    }
    int process_state = sys_get_process_status(pid);
    if(process_state != 0) {
        sys_set_fd(pid, STD_IN, STD_IN);
        sys_change_priority(pid, PRIORITY_HIGH);
        sys_unblock(pid);
        printf_color("[shell] running foreground process with pid=%d\n", COLOR_YELLOW, 0, pid);
        sys_wait_pid(pid);
    }
    return 0;
}

fd_t shell_pipe[2] = { 0 };

void shell() {
    
    sys_create_pipe(shell_pipe);

    print_help();

    do {
        printf_color("user@grupo20", COLOR_GREEN, 0x000000);
        putchar(':');
        printf_color("~", COLOR_BLUE, 0);
        printf("$ ");
        int break_line = 0;
        int i = 0;
        char command[MAX_BUF] = {0};
        while (!break_line) {
            char buf[2] = {0};
            sys_read(0, buf, 1);
            if (!(buf[0]== 0x08 && i == 0) && i < MAX_BUF)
                sys_write(1, buf, 1);
            if (buf[0] == '\n') {
                execute(command);
                break_line = 1;
            } else if (buf[0] == 0x08 && i > 0) {
                command[i - 1] = 0;
                i--;
            } else if (buf[0] != 0x08 && i < MAX_BUF) {
                command[i] = *buf;
                i++;
            }
        }
    } while (1);
}

static Program find_command(char *name) {
    for (int i = 0; i < sizeof(commands)/sizeof(commands[0]) ; i++)
    {
        if (strcmp(name, commands[i].title) == 0)
        {
            return commands[i].command;
        }
    }
    return NULL;
}

void execute(char command_buffer[]) {
    
    compact_whitespace(command_buffer);
    int argc_max = charcount(command_buffer, ' ') + 1;
    char* argv1[argc_max]; 
    char* argv2[argc_max]; 
    int argc1 = 0;
    int argc2 = 0;
    argv1[argc1++]=command_buffer;
    int piped = 0;
    int send_to_background = 0;



    for(int i = 0; command_buffer[i]; i++){
        switch (command_buffer[i])
        {
        case ' ':
            command_buffer[i] = 0;
            if(piped)
                argv2[argc2++] = &(command_buffer[i+1]); 
            else if(!send_to_background && command_buffer[i+1] != ' ')
                argv1[argc1++] = &(command_buffer[i+1]);
            break;
        case '|':
            command_buffer[i] = 0;
            if(piped) {
                printf_error("cant have more than one pipe in command\n");
                return;
            }
            piped = 1;
            break;
        case '&':
            command_buffer[i] = 0;
            if(!piped) 
                send_to_background = 1;
            break;
        default:
            break;
        }
    }
    if(argv1[argc1-1][0] == '\0') argc1--;

    Program process1 = find_command(argv1[0]);
    if(process1) {
        fd_t fds1[] = {STD_IN, STD_OUT, STD_ERR};
        const char *fg_bg = "foreground";
        pid_t pid_piped = 0;

        

        if(piped) {

            
            fd_t fds2[] = {shell_pipe[1], STD_OUT, STD_ERR};
            fds1[STD_OUT] = shell_pipe[0];

            Program process2 = find_command(argv2[0]);
            if(process2 == NULL) {
                printf_error("[shell] second comand is invalid '%s'\n", argv2[0]);
                return;
            }
            pid_piped = sys_create_process_fd(process2, argc2, argv2, fds2);
            printf("pid_piped: %d\n", pid_piped);
        }
        if(send_to_background) {
            fds1[STD_IN] = DEV_NULL;
            fg_bg = "background";
        }
        int pid = sys_create_process_fd(process1, argc1, argv1, fds1);
        printf("pid: %d\n", pid);
        printf_color("[shell] Running %s with pid=%d in %s...\n", COLOR_YELLOW, 0, argv1[0], pid, fg_bg);
        if(!send_to_background) {
            sys_change_priority(pid, PRIORITY_HIGH);
            sys_wait_pid(pid);
        }
        if(piped && sys_get_process_status(pid) == 0) {
            sys_wait_pid(pid_piped);
            // sys_close_pipe(shell_pipe[0]);
        }
    }
    else {
        printf_error("[shell] invalid command '%s', try 'help' command\n", argv1[0]);
    }    
}