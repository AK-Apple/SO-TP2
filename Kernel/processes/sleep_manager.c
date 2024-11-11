#include "process.h"
#include "time.h"
#include "IO.h"
#include <stdint.h>

// Código modificado de GeeksForGeeks: https://www.geeksforgeeks.org/c-program-to-implement-priority-queue/ 


typedef struct {
    uint64_t wakeup_time;
    pid_t pid;
} pq_item_t;

typedef struct {
    pq_item_t item[MAX_PROCESS_BLOCKS];
    int size;
} priority_queue_t;

priority_queue_t bedroom = { 0 } ;

void swap(pq_item_t* a, pq_item_t* b)
{
    pq_item_t temp = *a;
    *a = *b;
    *b = temp;
}

// Ordena la pq
void heapify_up(priority_queue_t* pq, int index)
{
    if (index
        && pq->item[(index - 1) / 2].wakeup_time > pq->item[index].wakeup_time) 
    {
        swap(&pq->item[(index - 1) / 2],
             &pq->item[index]);
        heapify_up(pq, (index - 1) / 2);
    }
}

void pq_enqueue(priority_queue_t* pq, uint64_t wakeup_time, pid_t pid)
{
    if (pq->size == MAX_PROCESS_BLOCKS) 
        return;

    pq->item[pq->size].wakeup_time = wakeup_time;
    pq->item[pq->size].pid = pid;
    pq->size++;
    heapify_up(pq, pq->size - 1);
}

// Ordena la pq
void heapify_down(priority_queue_t* pq, int index)
{
    uint64_t smallest = index;
    uint64_t left = 2 * index + 1;
    uint64_t right = 2 * index + 2;

    if (left < pq->size
        && pq->item[left].wakeup_time < pq->item[smallest].wakeup_time)
        smallest = left;

    if (right < pq->size
        && pq->item[right].wakeup_time < pq->item[smallest].wakeup_time)
        smallest = right;

    if (smallest != index) 
    {
        swap(&pq->item[index], &pq->item[smallest]);
        heapify_down(pq, smallest);
    }
}

pid_t pq_dequeue(priority_queue_t* pq)
{
    if (!pq->size) 
    {
        return INVALID_PID;
    }

    pq_item_t item = pq->item[0];
    pq->item[0] = pq->item[--pq->size];
    heapify_down(pq, 0);
    return item.pid;
}

uint64_t pq_peek(priority_queue_t* pq)
{
    if (!pq->size) 
    {
        return UINT64_MAX;
    }
    return pq->item[0].wakeup_time;
}

void remove_forced(priority_queue_t* pq, pid_t pid)
{
    for (int i = 0; i < pq->size; i++)
    {
        if (pq->item[i].pid == pid)
        {
            pq->item[i] = pq->item[--pq->size];
            heapify_up(pq, i);
            heapify_down(pq, 0);
            return;
        }
    }
}



void ticks_sleep(uint64_t ticks)
{
    uint64_t wakeup_time = ticks_elapsed() + ticks;
    // printf_error("wakeuptime = [%d]\n", wakeup_time);
    pq_enqueue(&bedroom, wakeup_time, get_pid());
    set_sleeping_state(1, get_pid());
    block(get_pid());
}

void seconds_sleep(uint64_t seconds)
{
    ticks_sleep(seconds * TICKS_PER_SECOND);
}

void wake_available()
{
    while(1)
    {
        uint64_t current_ticks = ticks_elapsed();
        uint64_t next_waking = pq_peek(&bedroom);
        if (next_waking != -1 && next_waking <= current_ticks)
        {
            pid_t to_wake = pq_dequeue(&bedroom);
            set_sleeping_state(0, to_wake);
            unblock(to_wake);
        }
        else return;
    }
}

void wake_forced(pid_t pid)
{
    remove_forced(&bedroom, pid);
}