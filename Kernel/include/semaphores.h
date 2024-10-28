#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <stdint.h>
#include "./../../Shared/shared.h"

typedef int64_t sem_t;

// abre un semáforo. 'value' es el valor inicial. Retorna 1 si ID es muy grande
uint64_t sem_open(sem_t id, uint64_t value);

int sem_wait(sem_t id);

int sem_post(sem_t id);

int sem_close(sem_t id);



#endif