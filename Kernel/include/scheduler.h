#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SCHEDULER_LOW_PRIORITY_RATE 3

#include <stdint.h>
#include "scheduling.h"
#include "shared.h"

typedef struct {
    int queue[PRIORITY_NONE][MAX_PROCESS_BLOCKS];
    int index[PRIORITY_NONE];
    int count[PRIORITY_NONE];
    int mid_priority_turns;
} Scheduler;

void scheduler_initialize();
int scheduler_next_pid();
int scheduler_insert(Priority priority, int pid);
int scheduler_remove(Priority priority, int pid);
int scheduler_get_quantum(Priority priority);
void scheduler_print();


#endif