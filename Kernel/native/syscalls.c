// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "video.h"
#include "IO.h"
#include "time.h"
#include "lib.h"
#include "interrupts.h"
#include "sound.h"
#include "process.h"
#include "memory_allocator.h"
#include "semaphores.h"
#include "pipes.h"
#include "scheduler.h"
#include "sleep_manager.h"
#include "registers.h"
#include "mvars.h"

typedef uint64_t (*Syscall)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

uint64_t int80Dispacher(uint64_t id, uint64_t param_1, uint64_t param_2, uint64_t param_3, uint64_t param_4, uint64_t param_5) {
    static const Syscall syscalls[] = {
        (Syscall)_hlt, // Syscall 0
        (Syscall)play_sound, // Syscall 1
        (Syscall)seconds_sleep,// Syscall 2
        (Syscall)sys_read, // Syscall 3
        (Syscall)sys_write, // Syscall 3
        (Syscall)sys_clear_screen, // Syscall 5
        (Syscall)put_pixel, // Syscall 6
        (Syscall)sys_new_size, // Syscall 7
        (Syscall)sys_get_time, // Syscall 8
        (Syscall)seconds_elapsed, // Syscall 9
        (Syscall)ticks_elapsed, // Syscall 10
        (Syscall)sys_get_size, // Syscall 11
        (Syscall)nosound, // Syscall 12
        (Syscall)get_latest_registers, // Syscall 13

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

        (Syscall)get_last_exit_code, // Syscall 30

        (Syscall)sem_open, // Syscall 31
        (Syscall)sem_wait, // Syscall 32
        (Syscall)sem_post, // Syscall 33
        (Syscall)sem_close, // Syscall 34

        (Syscall)set_foreground, // Syscall 35 
        (Syscall)create_pipe, // Syscall 36
        (Syscall)close_pipe_end, // Syscall 37

        (Syscall)ticks_sleep,  // Syscall 38
        (Syscall)set_stdin_options, // Syscall 39
        (Syscall)openMVar, // Syscall 40
        (Syscall)putMVar, // Syscall 41
        (Syscall)takeMVar, // Syscall 42

    };
    if(id >= sizeof(syscalls) / sizeof(syscalls[0])) {
        printf_error("[KERNEL] invalid syscall id:%lu\n", id);
        return 1;
    }
    return syscalls[id](param_1, param_2, param_3, param_4, param_5);
}