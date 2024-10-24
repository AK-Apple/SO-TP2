#include "lib.h"
#include "IO.h"
#include "interrupts.h"
#include "semaphores.h"
#include "bounded_queue.h"
#include "scheduling.h"

#define MAX_SEMAPHORES 32
#define NO_PRIORITY -1

uint64_t semaphoresExists[MAX_SEMAPHORES] = {0};
uint64_t semaphores[MAX_SEMAPHORES] = {0};
queue_t sem_queues[MAX_SEMAPHORES] = {0};
int64_t priorities[MAX_SEMAPHORES] = {0}; 


uint8_t lock = 1;

uint64_t valid_id(sem_t id)
{
    return  id > 0 && id < MAX_SEMAPHORES ;
}

void validate_id(sem_t id){
    if (!valid_id(id) || !semaphoresExists[id]) return;
}

uint8_t can_pass(sem_t id, uint64_t process)
{
    return priorities[id] == process || priorities[id] == NO_PRIORITY;
}

uint64_t sem_open(sem_t id, uint64_t value)
{
    if (!valid_id(id) || value < 0) return 0;

    acquire(&lock);

    if (!semaphoresExists[id])
    {
        semaphoresExists[id] = 1;
        semaphores[id] = value;
        init_queue(&sem_queues[id]);
        priorities[id] = NO_PRIORITY;
    }

    release(&lock);
    return 1 ;
}

void sem_close(sem_t id)
{
    validate_id(id);
    semaphoresExists[id] = 0;
}

void sem_wait(sem_t id)
{
    validate_id(id);

    uint8_t queued = 0;         // Sirve por si alguien decide hacer un unlock (en caso contrario, se podría poner un mismo proceso 2 veces en la queue)

    acquire(&lock);

    while(semaphores[id] == 0  || !can_pass(id, get_pid()) )
    {
        _cli();
        release(&lock);

        if (!queued)
        {
            enqueue(&sem_queues[id], get_pid());
        }            
        queued = 1;
        block(get_pid());                        
        // un sti al final del block reestaura el acceso a las interrupciones. Es importante que el bloqueo se realice sin interrupciones para prevenir que el proceso se bloquee por siempre
        acquire(&lock);
    }
    semaphores[id]--;
    release(&lock);
}


void sem_post(sem_t id)
{
    validate_id(id);

    acquire(&lock);

    int64_t to_unlock = 0;
    if (!dequeue(&sem_queues[id], &to_unlock))
    {
        priorities[id] = NO_PRIORITY;       // Darle prioridad al proceso que salió de la cola previene que el proceso que hace POST "se cole". 
                                            // DESVENTAJA: hay que recorrer todo el round robin, lo que puede ser medio lento. VENTAJA: no hay inanición
    }
    else
    {
        priorities[id] = to_unlock;
    }
    unlock(to_unlock);

    semaphores[id]++;

    release(&lock);

}
