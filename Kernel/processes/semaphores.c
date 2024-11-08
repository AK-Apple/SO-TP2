// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "lib.h"
#include "IO.h"
#include "interrupts.h"
#include "semaphores.h"
#include "bounded_queue.h"
#include "process.h"

#define MAX_SEMAPHORES (MAX_PROCESS_BLOCKS * 4)
#define NO_PRIORITY -1

uint64_t semaphore_exists[MAX_SEMAPHORES] = {0};
uint64_t semaphores[MAX_SEMAPHORES] = {0};
queue_t sem_queues[MAX_SEMAPHORES] = {0};
int64_t priorities[MAX_SEMAPHORES] = {0}; 


uint8_t lock = 1;

static uint64_t valid_id(sem_t id)
{
    return id > 0 && id < MAX_SEMAPHORES;
}

static uint64_t invalid_semaphore(sem_t id){
    return (!valid_id(id) || !semaphore_exists[id]);
}

static uint8_t can_pass(sem_t id, uint64_t process)
{
    return (priorities[id] == process || priorities[id] == NO_PRIORITY) && semaphores[id] > 0;
}

uint64_t sem_open(sem_t id, uint64_t value)
{
    if (!valid_id(id)) return SEM_ERROR;

    acquire(&lock);

    if (!semaphore_exists[id])
    {
        semaphore_exists[id] = 1;
        semaphores[id] = value;
        init_queue(&sem_queues[id]);
        priorities[id] = NO_PRIORITY;
    }

    release(&lock);

    return SEM_SUCCESS;
}

int sem_close(sem_t id)
{
    if (invalid_semaphore(id)) return SEM_ERROR;
    semaphore_exists[id] = 0;
    return SEM_SUCCESS;
}

int sem_wait(sem_t id)
{
    if (invalid_semaphore(id)) return SEM_ERROR;

    int pid = get_pid();
    acquire(&lock);
    if (!can_pass(id, pid))
    {
        enqueue(&sem_queues[id], pid);
    }            
    
    while(!can_pass(id, pid))
    {
        block_no_yield(pid);         
        release(&lock);
        yield(); 
        if(!semaphore_exists[id]) return SEM_ERROR;           
        acquire(&lock);
    }
    semaphores[id]--;
    release(&lock);

    return SEM_SUCCESS;
}

int sem_post(sem_t id)
{
    if (invalid_semaphore(id)) return SEM_ERROR;

    acquire(&lock);
    int64_t to_unblock = 0;
    if (!dequeue(&sem_queues[id], &to_unblock))
    {
        priorities[id] = NO_PRIORITY;       
        // Darle prioridad al proceso que salió de la cola previene que el proceso que hace POST "se cole". 
        // DESVENTAJA: hay que recorrer todo el round robin, lo que puede ser medio lento. VENTAJA: no hay inanición
    }
    else
    {
        priorities[id] = to_unblock;
    }
    unblock(to_unblock);
    semaphores[id]++;

    release(&lock);

    return SEM_SUCCESS;
}
