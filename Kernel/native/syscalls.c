#include "video.h"
#include "IO.h"
#include "time.h"
#include "lib.h"
#include "interrupts.h"
#include "sound.h"
#include "scheduling.h"
#include "fl_allocator.h"

typedef uint64_t (*Syscall)(uint64_t, uint64_t, uint64_t);

void MISSING_SYSCALL() {
    printf("[KERNEL] mising syscal");
}

uint64_t int80Dispacher(uint64_t id, uint64_t param_1, uint64_t param_2, uint64_t param_3) {
    static const Syscall syscalls[] = {
        _hlt, // #define SYS_HLT 0
        play_sound, // #define SYS_SOUND 1
        MISSING_SYSCALL,// #define SYS_SLEEP 2
        sys_read, // #define SYS_READ 3
        sys_write, // #define SYS_WRITE 4
        sys_clearScreen, // #define SYS_CLEAR_SCREEN 5
        putPixel, // #define SYS_PUT_PIXEL 6
        sys_new_size, // #define SYS_NEW_SIZE 7
        sys_getTime, // #define SYS_GET_TIME 8
        seconds_elapsed, // #define SYS_GET_ELAPSED_SECONDS 9
        ticks_elapsed, // #define SYS_GET_ELAPSED_TICKS 10
        sys_getSize, // #define SYS_WIDTH_HEIGHT 11
        nosound, //#define SYS_NOSOUND 12
        print_saved_regs, // #define SYS_GETREGS 13

        set_foreground_color, // #define SYS_SET_TEXT_COLOR 14

        create_process, // #define SYS_CREATE_PROCESS 15
        kill_process, //  #define SYS_KILL_PROCESS 16
        get_process_status, // #define SYS_PROCESS_STATUS 17
        get_pid, // #define SYS_GET_PID 18
        get_all_processes, // #define SYS_GET_ALL_PROCESSES 19
        change_priority, // #define SYS_CHANGE_PRIORITY 20
        block, // #define SYS_BLOCK 21
        unlock, // #define SYS_UNLOCK 22
        yield, // #define SYS_YIELD 23
        wait_pid, // #define SYS_WAITPID 24
        children_wait, // #define SYS_WAIT_CHILDREN 25
        exit, // #define SYS_EXIT 26

        fl_malloc, // #define SYS_MALLOC 27
        fl_free, // #define SYS_FREE 28
        fl_print_mem, // #define SYS_PRINT_MEM 29
    };
    if(id >= sizeof(syscalls) / sizeof(syscalls[0])) {
        printf("[KERNEL] invalid syscall id:");
        k_print_int_dec(id);
        putChar('\n');
    }
    return syscalls[id](param_1, param_2, param_3);
}