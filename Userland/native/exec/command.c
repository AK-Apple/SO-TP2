// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/command.h"
#include "../include/syscalls.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/eliminator.h"
#include "../include/test_util.h"
#include "../include/sounds.h"
#include "../include/string.h"
#include "shared.h"

#define REG_SIZE 17
#define SECONDS 0
#define MINUTES 2
#define HOURS 4
#define DAY_OF_THE_WEEK 6
#define DAY_OF_THE_MONTH 7
#define MONTH 8
#define YEAR 9

int wc(int argc, char *argv[]) {
    int input_char = 0;
    int line_count = 0;
    while((input_char = getchar()) != EOF) {
        line_count += (input_char == '\n');
    }
    printf("cantidad de lineas = %d\n", line_count);

    return 0;
}

int cat(int argc, char *argv[]) {
    int input_char = 0;
    do {
        input_char = getchar();
        if(input_char != '\b')
            putchar(input_char);
    } while(input_char != EOF);
    return 0;
}

int filter(int argc, char *argv[]) {
    int input_char = 0;
    while((input_char = getchar()) != EOF) {
        if(!is_vowel(input_char) && input_char != '\b')
            putchar(input_char);
    }
    return 0;
}

int64_t inforeg(uint64_t argc, char *argv[]) {
    StackedRegisters registers = {0};
    sys_get_registers(&registers);
    printf("\nRAX  : 0x%16lx", registers.rax);
    printf("\nRBX  : 0x%16lx", registers.rbx); 
    printf("\nRCX  : 0x%16lx", registers.rcx); 
    printf("\nRDX  : 0x%16lx", registers.rdx); 
    printf("\nRDI  : 0x%16lx", registers.rdi); 
    printf("\nRSI  : 0x%16lx", registers.rsi); 
    printf("\nRBP  : 0x%16lx", registers.rbp); 
    printf("\nRSP  : 0x%16lx", registers.rsp); 
    printf("\nR08  : 0x%16lx", registers.r8);
    printf("\nR09  : 0x%16lx", registers.r9);
    printf("\nR10  : 0x%16lx", registers.r10); 
    printf("\nR11  : 0x%16lx", registers.r11); 
    printf("\nR12  : 0x%16lx", registers.r12); 
    printf("\nR13  : 0x%16lx", registers.r13); 
    printf("\nR14  : 0x%16lx", registers.r14); 
    printf("\nR15  : 0x%16lx", registers.r15); 
    printf("\nRIP  : 0x%16lx", registers.rip); 
    printf("\nFLAG : 0x%16lx", registers.rflags);
    printf("\nCS   : 0x%16lx", registers.cs);
    printf("\nSS   : 0x%16lx", registers.ss);
    printf("\n");
    return 0;
}

void print_processes_state() {
    static char *PROCESS_STATE_STRING[] = {"UNKNOWN", "RUNNING", "READY  ", "BLOCKED"};
    static uint64_t PROCESS_STATE_COLOR[] = {0x00999999, 0x0000FF00, 0x00CCDD00, 0x00FF0000};
    static char *PRIORITY_STRING[] = {"LOW ", "MID ", "HIGH", "NONE"};
    printf("pid : ppid : prio : stack_pointer_64 : base_pointer_64  : status  : process_name\n");
    ProcessInfo ps = {0};
    sys_get_processes_info(&ps);
    if(ps.entries == NULL) {
        printf_error("no se pudo imprimir process info porque se quedo sin memoria dinamica\n");
        return;
    }
    for (int i = 0; i < ps.count; i++)
    {
        printf("%3d : %4ld : ", ps.entries[i].pid, ps.entries[i].ppid);
        int priority = ps.entries[i].priority;
        printf_color("%s", PROCESS_STATE_COLOR[priority+1], 0, PRIORITY_STRING[priority]);
        printf(" : %16lx : %16lx : ", ps.entries[i].rsp, ps.entries[i].rbp);
        uint64_t process_state = ps.entries[i].status;
        printf_color(PROCESS_STATE_STRING[process_state], PROCESS_STATE_COLOR[process_state], 0);
        printf(" : %s ", ps.entries[i].name);
        if(ps.entries[i].pid == ps.foreground_pid)
            printf_color("(foreground)", COLOR_GREEN, 0);
        putchar('\n');
    }
    sys_memory_free(ps.entries);
}

int64_t echo_cmd(uint64_t argc, char *argv[]) {
    for(int i = 1; i < argc; i++) {
        int last_cut_index = 0;
        for(int j = 0; argv[i][j]; j++) {
            if(argv[i][j] == '#' && argv[i][j+1] == '?') {
                argv[i][j] = '\0';
                printf("%s%ld", &argv[i][last_cut_index], sys_get_exit_code());
                j++;
                last_cut_index = j + 1;
            }
        }
        printf("%s ", &argv[i][last_cut_index]);
    }
    printf("\n");
    putchar(EOF);
    return 0;
}

void play_music_cmd(uint64_t argc, char *argv[]) 
{
    uint64_t song_id = 1;
    if(argc >= 2) {
        song_id = satoi(argv[1]);
        if(song_id < 1 || song_id > 3) {
            song_id = 1;
        }
    }
    play_song_continuous(song_id);
}

void play_infinite_music_cmd(uint64_t argc, char *argv[]) 
{
    uint64_t song_id = 1;
    if(argc >= 2) {
        song_id = satoi(argv[1]);
        if(song_id < 1 || song_id > 3) {
            song_id = 1;
        }
    }
    while(1)
    {
        play_song_continuous(song_id);
    }
}

void divide(uint64_t argc, char *argv[]) {
    if(argc == 3) {
        int64_t a, b;
        a = satoi(argv[1]);    
        b = satoi(argv[2]);
        int64_t res = a / b;
        printf("%ld / %ld = %ld\n", a, b, res);    
    }
    else {
        printf_error("se recibio: '%s' '%s'\nusage: div <a> <b>\n", argv[1], argv[2]);
    }
}

void invalidOpcode() {
    printf("executing invalid opcode...\n");
    static uint8_t invalidOpcodes[] = {0x06, 0x07};
    ((void (*)()) invalidOpcodes)();
}

void changeSize(uint64_t argc, char *argv[]) {
    uint32_t new_size = 1;
    if (argc >= 2) {
        new_size = satoi(argv[1]);
        if(new_size < 1 || new_size > 5) {
            new_size = 1;
        }
    }
    sys_new_size(new_size);
}

void kill_process(uint64_t argc, char *argv[]) {
    pid_t pid = satoi(argv[1]);
    printf("[killing process] pid=%d\n", pid);
    if(argc < 2) {
        printf_error("invalid arguments\n"); 
        return;
    }
    if(pid < 1) {
        printf_error("invalid pid\n");
        return;
    }
    int recursive = 0;
    if(argc >= 3) {
        recursive = satoi(argv[2]) == 1;
    }
    sys_kill_process(pid, recursive);
}

void print_time() {
    int hours = sys_time(HOURS);
    int minutes = sys_time(MINUTES);
    int day = sys_time(DAY_OF_THE_MONTH);
    int month = sys_time(MONTH);
    int year = sys_time(YEAR) + 2000;
    printf("%2d:%2d, %2d/%2d/%4d\n", hours, minutes, day, month, year);
}