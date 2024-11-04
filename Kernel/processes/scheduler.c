#include "scheduler.h"
#include "IO.h"
#include "lib.h"

Scheduler scheduler = {0};

void scheduler_initialize() {
    memset(&scheduler, 0, sizeof(Scheduler));
}

int scheduler_next_pid() {
    if(scheduler.high_priority_index >= scheduler.high_priority_count) {
        scheduler.high_priority_index = 0;

        if(scheduler.mid_priority_turns >= SCHEDULER_LOW_PRIORITY_RATE || scheduler.mid_priority_count == 0) {
            scheduler.mid_priority_turns = 0;
            scheduler.low_priority_index %= scheduler.low_priority_count;
            return scheduler.low_priority[scheduler.low_priority_index++];
        }

        scheduler.mid_priority_index %= scheduler.mid_priority_count;
        scheduler.mid_priority_turns++;
        return scheduler.mid_priority[scheduler.mid_priority_index++];
    }

    return scheduler.high_priority[scheduler.high_priority_index++];
}

int scheduler_insert(Priority priority, int pid) {
    switch (priority)
    {
    case PRIORITY_HIGH:
        if(scheduler.high_priority_count >= MAX_PROCESS_BLOCKS) {
            printf_error("schduler cant add\n");
            return -1;
        }
        scheduler.high_priority[scheduler.high_priority_count++] = pid;
        break;
    case PRIORITY_MID:
        if(scheduler.mid_priority_count >= MAX_PROCESS_BLOCKS) {
            printf_error("schduler cant add\n");
            return -1;
        }
        scheduler.mid_priority[scheduler.mid_priority_count++] = pid;
        break;
    case PRIORITY_LOW:
        if(scheduler.low_priority_count >= MAX_PROCESS_BLOCKS) {
            printf_error("schduler cant add\n");
            return -1;
        }
        scheduler.low_priority[scheduler.low_priority_count++] = pid;
        break;
    default:
        printf_error("inexistent priority\n");
        return -1;
    }
    return 0;
}

int scheduler_remove(Priority priority, int pid) {
    switch (priority)
    {
    case PRIORITY_HIGH:
        for(int i = 0; i < scheduler.high_priority_count; i++) {
            if(scheduler.high_priority[i] == pid) {
                scheduler.high_priority[i] = scheduler.high_priority[--scheduler.high_priority_count];
                return 0;
            }
        }
        break;
    case PRIORITY_MID:
        for(int i = 0; i < scheduler.mid_priority_count; i++) {
            if(scheduler.mid_priority[i] == pid) {
                scheduler.mid_priority[i] = scheduler.mid_priority[--scheduler.mid_priority_count];
                return 0;
            }
        }
        break;
    case PRIORITY_LOW:
        for(int i = 0; i < scheduler.low_priority_count; i++) {
            if(scheduler.low_priority[i] == pid) {
                scheduler.low_priority[i] = scheduler.low_priority[--scheduler.low_priority_count];
                return 0;
            }
        }
        break;
    default:
        break;
    }
    return -1;
}

int scheduler_get_quantum(Priority priority) {
    switch (priority)
    {
    case PRIORITY_HIGH:
        return 1;
    case PRIORITY_MID:
        return 0;
    case PRIORITY_LOW:
        return 0;
    }
    return 0;
}

void scheduler_print() {
    printf("\nHIGH PRIORITY: ");
    for(int i = 0; i < scheduler.high_priority_count; i++) {
        printf("%d%c", scheduler.high_priority[i], scheduler.high_priority_index == i ? ']' : ' ');
    }
    printf("\nMID PRIORITY: ");
    for(int i = 0; i < scheduler.mid_priority_count; i++) {
        printf("%d%c", scheduler.mid_priority[i], scheduler.mid_priority_index == i ? ']' : ' ');
    }
    printf("\nLOW PRIORITY: ");
    for(int i = 0; i < scheduler.low_priority_count; i++) {
        printf("%d%c", scheduler.low_priority[i], scheduler.low_priority_index == i ? ']' : ' ');
    }
    printf("\n");
}



/* %3
HIGH: 1 2
MID:  3 4 5
LOW:  6 7

1 2 3 1 2 4 1 2 5 1 2 6 1 2 3 1 2 4 1 2 5 1 2 7 1 2 3 1 2 4
*/
/* %3
HIGH: 1
MID: 2 3
LOW: 4 5 6

1 2 1 3 1 2 1 4 1 2 1 3 1 2 1 5 1 
*/
