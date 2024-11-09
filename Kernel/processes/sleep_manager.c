#include "process.h"
#include "time.h"
#include "IO.h"

#define UINT64_MAX -1

typedef struct {
    uint64_t wakeup_time;
    pid_t pid;
} pq_item_t;

// Define PriorityQueue structure
typedef struct {
    pq_item_t item[MAX_PROCESS_BLOCKS];
    int size;
} priority_queue_t;

priority_queue_t bedroom = { 0 } ;

// Define swap function to swap two integers
void swap(pq_item_t* a, pq_item_t* b)
{
    pq_item_t temp = *a;
    *a = *b;
    *b = temp;
}

// Define heapifyUp function to maintain heap property
// during insertion
void heapifyUp(priority_queue_t* pq, int index)
{
    if (index
        && pq->item[(index - 1) / 2].wakeup_time > pq->item[index].wakeup_time) 
    {
        swap(&pq->item[(index - 1) / 2],
             &pq->item[index]);
        heapifyUp(pq, (index - 1) / 2);
    }
}

// Define enqueue function to add an item to the queue
void pq_enqueue(priority_queue_t* pq, uint64_t wakeup_time, pid_t pid)
{
    if (pq->size == MAX_PROCESS_BLOCKS) 
    {
        // printf("Priority queue is full\n");
        return;
    }

    pq->item[pq->size].wakeup_time = wakeup_time;
    pq->item[pq->size].pid = pid;
    pq->size++;
    heapifyUp(pq, pq->size - 1);
}

// Define heapifyDown function to maintain heap property
// during deletion
void heapifyDown(priority_queue_t* pq, int index)
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
        heapifyDown(pq, smallest);
    }
}

// Define dequeue function to remove an item from the queue
pid_t pq_dequeue(priority_queue_t* pq)
{
    if (!pq->size) 
    {
        // printf("Priority queue is empty\n");
        return INVALID_PID;
    }

    pq_item_t item = pq->item[0];
    pq->item[0] = pq->item[--pq->size];
    heapifyDown(pq, 0);
    return item.pid;
}

// Define peek function to get the top item from the queue
uint64_t pq_peek(priority_queue_t* pq)
{
    if (!pq->size) 
    {
        // printf("Priority queue is empty\n");
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
            heapifyUp(pq, i);
            heapifyDown(pq, 0);
            return;
        }
    }
}



void ticks_sleep_2(uint64_t ticks)
{
    uint64_t wakeup_time = ticks_elapsed() + ticks;
    // printf_error("wakeuptime = [%d]\n", wakeup_time);
    pq_enqueue(&bedroom, wakeup_time, get_pid());
    set_sleeping_state(1, get_pid());
    block(get_pid());
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