// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "scheduler.h"
#include "IO.h"
#include "lib.h"
#include "time.h"
#include "interrupts.h"

Scheduler scheduler = {0};

void scheduler_initialize() 
{
    memset(&scheduler, 0, sizeof(Scheduler));
}

void yield()
{
    scheduler.remaining_quantum = 0;
    dont_handle_ticks();
    force_timer_tick();
}

pid_t scheduler_next_pid() 
{
    if(scheduler.index[PRIORITY_HIGH] >= scheduler.count[PRIORITY_HIGH]) 
    {
        scheduler.index[PRIORITY_HIGH] = 0;
        scheduler.remaining_quantum = 0;

        if(scheduler.mid_priority_turns >= SCHEDULER_LOW_PRIORITY_RATE || scheduler.count[PRIORITY_MID] == 0) 
        {
            scheduler.mid_priority_turns = 0;
            scheduler.index[PRIORITY_LOW] %= scheduler.count[PRIORITY_LOW];
            return scheduler.queue[PRIORITY_LOW][scheduler.index[PRIORITY_LOW]++];
        }

        scheduler.index[PRIORITY_MID] %= scheduler.count[PRIORITY_MID];
        scheduler.mid_priority_turns++;
        return scheduler.queue[PRIORITY_MID][scheduler.index[PRIORITY_MID]++];
    }

    scheduler.remaining_quantum = 1;
    return scheduler.queue[PRIORITY_HIGH][scheduler.index[PRIORITY_HIGH]++];
}

int scheduler_insert(Priority priority, pid_t pid) 
{
    if(priority > PRIORITY_HIGH || scheduler.count[priority] >= MAX_PROCESS_BLOCKS)
        return -1;
    scheduler.queue[priority][scheduler.count[priority]++] = pid;

    return scheduler.count[priority] - 1;
}

int scheduler_remove(Priority priority, pid_t pid) 
{
    if(priority > PRIORITY_HIGH)
        return -1;
    for(int i = 0; i < scheduler.count[priority]; i++) 
    {
        if(scheduler.queue[priority][i] == pid) {
            scheduler.queue[priority][i] = scheduler.queue[priority][--scheduler.count[priority]];
            return 0;
        }
    }

    return -1;
}

int scheduler_consume_quantum() 
{
    return scheduler.remaining_quantum--;
}
