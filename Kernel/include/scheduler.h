#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SCHEDULER_LOW_PRIORITY_RATE 3

#include <stdint.h>
#include "scheduling.h"
#include "shared.h"

typedef struct {
    int high_priority[MAX_PROCESS_BLOCKS];
    int mid_priority[MAX_PROCESS_BLOCKS];
    int low_priority[MAX_PROCESS_BLOCKS];
    int high_priority_index;
    int mid_priority_index;
    int low_priority_index;
    int high_priority_count;
    int mid_priority_count;
    int low_priority_count;
    int mid_priority_turns;
} Scheduler;

void scheduler_initialize();
int scheduler_next_pid();
int scheduler_insert(Priority priority, int pid);
int scheduler_remove(Priority priority, int pid);
int scheduler_get_quantum(Priority priority);
void scheduler_print();


#endif