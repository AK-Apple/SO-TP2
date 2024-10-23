#include "lib.h"
#include "IO.h"
#include "interrupts.h"
#include "semaphores.h"
#include "bounded_queue.h"
#include "scheduling.h"

#define MAX_SEMAPHORES 32

#define NOT_DEFINED -1      // No puede ser 0

#define NO_PRIORITY MAX_SEMAPHORES

uint64_t semaphoresExists[MAX_SEMAPHORES] = {0};
uint64_t semaphores[MAX_SEMAPHORES] = {0};
queue_t sem_queues[MAX_SEMAPHORES] = {0};
int64_t priorities[MAX_SEMAPHORES] = {0};     // se pasaría a -1 a 0xFFFF...

// TODO: No sé cómo manejar los valores de retorno. En este caso, 0 = success y 1 = failure, pero quizás convenga hacerlo al revés

uint8_t lock = 1;

uint64_t valid_id(sem_t id)
{
    return semaphoresExists[id] && id > 0 && id < MAX_SEMAPHORES ;
}

// TODO: usar can_pass y asignar -1 a priorities cuando no haya nadie esperando
uint8_t can_pass(sem_t id, uint64_t process)
{
    return priorities[id] == process || priorities[id] == NO_PRIORITY;
}

uint64_t sem_open(sem_t id, uint64_t value)
{
    printf("semaphoreExists: %d", semaphoresExists[id]);
    if (id < 0 && id >= MAX_SEMAPHORES) return 0;
    if (value < 0) return 0;

    acquire(&lock);

    if(!semaphoresExists[id])
    {
        printf("sem created\n");
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
    if (!valid_id(id)) printf("Invalid semaphore id\n");
    semaphoresExists[id] = 0;
}

void sem_wait(sem_t id)
{
    printf("wait\n");
    if (!valid_id(id)) printf("Invalid semaphore id\n");
    uint8_t queued = 0;         // Puede servir por si alguien decide hacer un unlock (en caso contrario, se podría poner un mismo proceso 2 veces en la queue)
    acquire(&lock);
    printf("priorities before While: %d\n", priorities[id]);
    printf("semaphore value: %d\n", semaphores[id]);
    printf("Proceso que hace wait: %d\n", get_pid());
    while(semaphores[id] == 0  || (priorities[id] != NO_PRIORITY && priorities[id] != get_pid()) )
    {
        printf("Priorities: %d\n", priorities[id]);
        _cli();

        release(&lock);

        if (!queued)         // queued forma parte de otra zona crítica. Conviene incorporarlo todo en la función queue y usar otro lock/unlock
        {
            enqueue(&sem_queues[id], get_pid());
        }            
        queued = 1;
        printf("About to block\n");
        block(get_pid());                         // Problema: ¿Qué pasa si se hace QUEUE --> DEQUEUE --> BLOCK? (Deadlock) Conviene que BLOCK sea lo que haga release al final o poner algún CLI
        acquire(&lock);
        // sti al final del block   (otra solución: poner sti al final del block)
    }
    semaphores[id]--;
    printf("Proceso que hizo wait (deberia ser el mismo): %d\n", get_pid());
    release(&lock);
}


void sem_post(sem_t id)
{
    if (!valid_id(id)) printf("Invalid semaphore id\n");

    acquire(&lock);
    
    semaphores[id]++;
    printf("Proceso %d hace sem_post\n", get_pid());
    // si el semáforo es 0, entonces hace unlocks. Si no, creo que no hace falta
    if (semaphores[id]==0)
    {
        int64_t to_unlock = 0;
        if (!dequeue(&sem_queues[id], &to_unlock))
        {
            priorities[id] = NO_PRIORITY;            // -1 deshabilita las prioridades
            printf("anarquia\n");
        }
        else
        {
            priorities[id] = to_unlock;
        }
        printf("to_unlock: %d\n", to_unlock);
        printf("priorities: %d\n", priorities[id]);
        unlock(to_unlock);
    }

    release(&lock);

    // TODO: Obs: Este proceso (y cualquiera que no esté bloqueado) puede "colarse". Para evitar esto habría que darle "prioridad" al último proceso que sale
}

