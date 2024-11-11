#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SCHEDULER_LOW_PRIORITY_RATE 3

#include <stdint.h>
#include "process.h"
#include "shared.h"

typedef struct {
    int queue[PRIORITY_NONE][MAX_PROCESS_BLOCKS];
    int index[PRIORITY_NONE];
    int count[PRIORITY_NONE];
    int mid_priority_turns;
    int remaining_quantum;
} Scheduler;

void scheduler_initialize();
pid_t scheduler_next_pid();
int scheduler_insert(Priority priority, pid_t pid);
int scheduler_remove(Priority priority, pid_t pid);
int scheduler_consume_quantum();
void yield();


#endif