#include "video.h"
#include "IO.h"
#include "time.h"
#include "lib.h"
#include "interrupts.h"
#include "sound.h"
#include "scheduling.h"
#include "memory_allocator.h"
#include "semaphores.h"


typedef uint64_t (*Syscall)(uint64_t, uint64_t, uint64_t);

void MISSING_SYSCALL() {
    printf_error("[KERNEL] missing syscall\n");
}

void syscall_halt() {
    set_current_quantum(0);
    _hlt();
}

uint64_t int80Dispacher(uint64_t id, uint64_t param_1, uint64_t param_2, uint64_t param_3) {
    static const Syscall syscalls[] = {
        (Syscall)syscall_halt, // #define SYS_HLT 0
        (Syscall)play_sound, // #define SYS_SOUND 1
        (Syscall)seconds_sleep,// #define SYS_SLEEP 2
        (Syscall)sys_read, // #define SYS_READ 3
        (Syscall)sys_write, // #define SYS_WRITE 4
        (Syscall)sys_clearScreen, // #define SYS_CLEAR_SCREEN 5
        (Syscall)putPixel, // #define SYS_PUT_PIXEL 6
        (Syscall)sys_new_size, // #define SYS_NEW_SIZE 7
        (Syscall)sys_getTime, // #define SYS_GET_TIME 8
        (Syscall)seconds_elapsed, // #define SYS_GET_ELAPSED_SECONDS 9
        (Syscall)ticks_elapsed, // #define SYS_GET_ELAPSED_TICKS 10
        (Syscall)sys_getSize, // #define SYS_WIDTH_HEIGHT 11
        (Syscall)nosound, //#define SYS_NOSOUND 12
        (Syscall)print_saved_regs, // #define SYS_GETREGS 13

        (Syscall)set_foreground_color, // #define SYS_SET_TEXT_COLOR 14

        (Syscall)create_process, // #define SYS_CREATE_PROCESS 15
        (Syscall)kill_process, //  #define SYS_KILL_PROCESS 16
        (Syscall)get_process_status, // #define SYS_PROCESS_STATUS 17
        (Syscall)get_pid, // #define SYS_GET_PID 18
        (Syscall)get_all_processes, // #define SYS_GET_ALL_PROCESSES 19
        (Syscall)change_priority, // #define SYS_CHANGE_PRIORITY 20
        (Syscall)block, // #define SYS_BLOCK 21
        (Syscall)unlock, // #define SYS_UNLOCK 22
        (Syscall)yield, // #define SYS_YIELD 23
        (Syscall)wait_pid, // #define SYS_WAITPID 24
        (Syscall)children_wait, // #define SYS_WAIT_CHILDREN 25
        (Syscall)exit, // #define SYS_EXIT 26

        (Syscall)memory_alloc, // #define SYS_MALLOC 27
        (Syscall)memory_free, // #define SYS_FREE 28
        (Syscall)memory_info, // #define SYS_PRINT_MEM 29,
        (Syscall)MISSING_SYSCALL, // #define SYS_LARGEST_FREE_BLOCK 30 (eliminada)

        (Syscall)sem_open, // #define SYS_SEM_OPEN 31
        (Syscall)sem_wait, // #define SYS_SEM_WAIT 32
        (Syscall)sem_post, // #define SYS_SEM_POST 33
        (Syscall)sem_close, // #define SYS_SEM_CLOSE 34
    };
    if(id >= sizeof(syscalls) / sizeof(syscalls[0])) {
        printf_error("[KERNEL] invalid syscall id:%d\n", id);
        return 1;
    }
    return syscalls[id](param_1, param_2, param_3);
}