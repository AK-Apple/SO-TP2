#ifndef MVARS_H
#define MVARS_H

#include <stdint.h>
#include <stddef.h>
#include "semaphores.h"
#include "shared.h"


/*
rellena la estructura mvar con los datos de una mvar existente, o crea una nueva instancia de ser necesario.
Suponga que existe una única mvar para todo el kernel, que puede o no estar inicializada.
*/
void openMVar(mvar_t * mvar);
// create semaphore
// isEmpty1 = 1
// isFull = 0

/*
se bloquea hasta que la mvar esté vacía y después escribe un valor.
*/
void putMVar(mvar_t * mvar, int value);
// down isEmpty
// up isFull


/*
se bloquea hasta que la mvar tenga algo para leer y la vacía.
*/
int takeMVar(mvar_t * mvar);
// down isFull
// up isEmpty

#endif