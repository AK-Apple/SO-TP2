#include "../include/command.h"
#include "../include/syscalls.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/eliminator.h"
#include "../include/test_util.h"
#include "../include/sounds.h"

#define REG_SIZE 17
#define SECONDS 0
#define MINUTES 2
#define HOURS 4
#define DAY_OF_THE_WEEK 6
#define DAY_OF_THE_MONTH 7
#define MONTH 8
#define YEAR 9

extern int active_pid;

void play_music_cmd(uint64_t argc, char *argv[]) {
    uint64_t song_id = 1;
    if(argc >= 2) {
        song_id = satoi(argv[1]);
        if(song_id < 1 || song_id > 3) {
            song_id = 1;
        }
    }
    play_song(song_id);
    while (next_part()); // esto bloquea la consola, quiza en un futura hacer que corra en background
}

void clear() {
    sys_clear();
}

void divide(uint64_t argc, char *argv[]) {
    if(argc == 3) {
        int64_t a, b;
        a = satoi(argv[1]);    
        b = satoi(argv[2]);
        int64_t res = a / b;
        printf("%d / %d = %d\n", a, b, res);    
    }
    else {
        printf_error("div <a> <b>\n");
    }
}

void invalidOpcode() {
    printf("executing invalid opcode...\n");
    static uint8_t invalidOpcodes[] = {0x06, 0x07}; // PUSH ES and POP ES opcodes in 32-bit. disabled in 64-bit => invalid opcode
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

void print_process_state() {
    sys_print_all_processes();
}

void kill_process(uint64_t argc, char *argv[]) {
    int pid = satoi(argv[1]);
    printf("[killing process] pid=%d\n", pid);
    if(argc != 2) {
        printf_error("invalid arguments\n"); 
        return;
    }
    if(pid < 1) {
        printf_error("invalid pid\n");
        return;
    }
    sys_kill_process(pid);
}
int64_t test_mman_cmd(uint64_t argc, char *argv[]) {
    int pid = sys_create_process(get_test_mman(), argc, argv);
    active_pid = pid; 
}
int64_t test_processes_cmd(uint64_t argc, char *argv[]) {
    int pid = sys_create_process(get_test_processes(), argc, argv);
    active_pid = pid; 
}
int64_t test_prio_cmd(uint64_t argc, char *argv[]) {
    int pid = sys_create_process(get_test_prio(), argc, argv);
    active_pid = pid;
}

int64_t test_sync_cmd(uint64_t argc, char *argv[]) {
    int pid = sys_create_process(get_test_sync(), argc, argv);
    active_pid = pid;
}



void print_time() {

    int hours = sys_time(HOURS);
    // hours -= 3;          // el tema con pasar a horario de argentina es que habría que cambiar el día (y hasta el año si es 31/12) también, y es todito un lío
    // if(hours <= 0){
    //     hours += 24;
    // }
    printInt(hours);
    printf(":");
    int minutes = sys_time(MINUTES);
    if(minutes < 10)
        printf("0");
    printInt(minutes);
    printf(", ");
    printInt(sys_time(DAY_OF_THE_MONTH));
    printf("/");
    printInt(sys_time(MONTH));
    printf("/");
    printInt(sys_time(YEAR) + 2000);
    printf("\n");
}