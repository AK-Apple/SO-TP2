// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "video.h"
#include "IO.h"
#include "time.h"
#include "lib.h"
#include "interrupts.h"
#include "sound.h"
#include "scheduling.h"
#include "memory_allocator.h"
#include "semaphores.h"
#include "pipes.h"


typedef uint64_t (*Syscall)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

void MISSING_SYSCALL() {
    printf_error("[KERNEL] missing syscall\n");
}

void syscall_halt() {
    set_current_quantum(0);
    _hlt();
}

uint64_t int80Dispacher(uint64_t id, uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5) {
    static const Syscall syscalls[] = {
        (Syscall)syscall_halt, // Syscall 0
        (Syscall)play_sound, // Syscall 1
        (Syscall)seconds_sleep,// Syscall 2
        (Syscall)sys_read, // Syscall 3
        (Syscall)sys_write, // Syscall 3
        (Syscall)sys_clearScreen, // Syscall 5
        (Syscall)putPixel, // Syscall 6
        (Syscall)sys_new_size, // Syscall 7
        (Syscall)sys_getTime, // Syscall 8
        (Syscall)seconds_elapsed, // Syscall 9
        (Syscall)ticks_elapsed, // Syscall 10
        (Syscall)sys_getSize, // Syscall 11
        (Syscall)nosound, // Syscall 12
        (Syscall)print_saved_regs, // Syscall 13

        (Syscall)set_foreground_color, // Syscall 14

        (Syscall)create_process, // Syscall 15
        (Syscall)kill_process, //  Syscall 16
        (Syscall)get_process_status, // Syscall 17
        (Syscall)get_pid, // Syscall 18
        (Syscall)get_all_processes, // Syscall 19
        (Syscall)change_priority, // Syscall 20
        (Syscall)block, // Syscall 21
        (Syscall)unblock, // Syscall 22
        (Syscall)yield, // Syscall 23
        (Syscall)wait_pid, // Syscall 24
        (Syscall)sys_set_fd, // Syscall 25
        (Syscall)exit, // Syscall 26

        (Syscall)memory_alloc, // Syscall 27
        (Syscall)memory_free, // Syscall 28
        (Syscall)memory_info, // Syscall 29
        (Syscall)MISSING_SYSCALL, // #define SYS_LARGEST_FREE_BLOCK 30 (eliminada)

        (Syscall)sem_open, // Syscall 31
        (Syscall)sem_wait, // Syscall 32
        (Syscall)sem_post, // Syscall 33
        (Syscall)sem_close, // Syscall 34

        (Syscall)create_pipe, // Syscall 35
        (Syscall)request_pipe, // Syscall 36
        (Syscall)close_pipe, // Syscall 37

    };
    if(id >= sizeof(syscalls) / sizeof(syscalls[0])) {
        printf_error("[KERNEL] invalid syscall id:%d\n", id);
        return 1;
    }
    return syscalls[id](param_1, param_2, param_3, param_4, param_5);
}