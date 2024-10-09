#include "video.h"
#include "IO.h"
#include "time.h"
#include "lib.h"
#include "interrupts.h"
#include "sound.h"
#include "scheduling.h"
#include "fl_allocator.h"

#define SYS_HLT 0
#define SYS_SOUND 1
#define SYS_SLEEP 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_CLEAR_SCREEN 5
#define SYS_PUT_PIXEL 6
#define SYS_NEW_SIZE 7
#define SYS_GET_TIME 8
#define SYS_GET_ELAPSED_SECONDS 9
#define SYS_GET_ELAPSED_TICKS 10
#define SYS_WIDTH_HEIGHT 11
#define SYS_NOSOUND 12
#define SYS_GETREGS 13

#define SYS_GET_MEM 14
#define SYS_CREATE_PROCESS 15
#define SYS_KILL_PROCESS 16
#define SYS_PROCESS_STATUS 17
#define SYS_GET_PID 18
#define SYS_GET_ALL_PROCESSES 19
#define SYS_CHANGE_PRIORITY 20
#define SYS_BLOCK 21
#define SYS_UNLOCK 22
#define SYS_YIELD 23
#define SYS_WAITPID 24

uint64_t registers[18] = {0};

uint64_t int80Dispacher(uint64_t id, uint64_t param_1, uint64_t param_2, uint64_t param_3){
    switch (id) {
        case SYS_SOUND:
            play_sound(param_1);
            return 1;
        case SYS_NOSOUND:
            nosound();
            return 1;
        case SYS_WRITE:
            sys_write(param_1, (char *) param_2, param_3);
            return 1;
        case SYS_READ:
            return sys_read(param_1, (char *) param_2, param_3);
        case SYS_CLEAR_SCREEN:
            sys_clearScreen();
            return 1;
        case SYS_PUT_PIXEL:
            putPixel(param_1, param_2, param_3);
            return 1;
        case SYS_NEW_SIZE:
            sys_new_size(param_1);
            return 1;
        case SYS_GET_TIME:
            return sys_getTime(param_1);
        case SYS_GET_ELAPSED_SECONDS:
            return seconds_elapsed();
        case SYS_HLT:
            _hlt();
            return 1;
        case SYS_GET_ELAPSED_TICKS:
            return ticks_elapsed();
        case SYS_WIDTH_HEIGHT:
            return sys_getSize(param_1);
        case SYS_GETREGS:
            sys_registers();
            return 1;
        case SYS_GET_MEM:
            return fl_malloc();
        case SYS_CREATE_PROCESS:
            create_process((char *) param_1, param_2, (char **) param_3);
            return 1;
        case SYS_KILL_PROCESS:
            kill_process(param_1);
            return 1;
        case SYS_PROCESS_STATUS:
            return get_process_status(param_1);
        case SYS_GET_PID:
            return get_pid();
        case SYS_GET_ALL_PROCESSES:
            get_all_processes();
            return 1; 

        case SYS_CHANGE_PRIORITY:
            change_priority(param_1, param_2);
            return 1;
        case SYS_BLOCK:
            block();
            return 1;
        case SYS_UNLOCK:
            unlock(param_1);
            return 1;
        case SYS_YIELD:
            yield();
            return 1;
        case SYS_WAITPID:
            wait_pid();
            return 1;
    }
    return 0;
}