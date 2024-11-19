#include "mvars.h"
#include "semaphores.h"

#define SEM_ID_IS_FULL 20
#define SEM_ID_IS_EMPTY (SEM_ID_IS_FULL + 1)

uint8_t mvar_value = 0;

void openMVar(mvar_t * mvar) {

    mvar->sem_is_empty = SEM_ID_IS_EMPTY;
    mvar->sem_is_full = SEM_ID_IS_FULL;
    mvar->value = &mvar_value;

    sem_open(mvar->sem_is_empty, 1);
    sem_open(mvar->sem_is_full, 0);
}

/*
se bloquea hasta que la mvar esté vacía y después escribe un valor.
*/
void putMVar(mvar_t * mvar, int value) {
    sem_wait(mvar->sem_is_empty);
    *(mvar->value) = value;
    sem_post(mvar->sem_is_full);
}


/*
se bloquea hasta que la mvar tenga algo para leer y la vacía.
*/
int takeMVar(mvar_t * mvar) {
    sem_wait(mvar->sem_is_full);
    int aux = *(mvar->value);
    *(mvar->value) = 0;
    sem_post(mvar->sem_is_empty);
    return aux;
}

/*
putvar

1) si mvar no esta vacio, entonces bloquear al proceso actual que lo llamo
    down isFull
    -- 
    up isEmpty


takevar
*/