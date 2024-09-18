#ifndef INTERRUPS_H_
#define INTERRUPS_H_

#include "idtLoader.h"
#include <stdint.h>

void _irq00Handler(void);
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);

void _int80Handler(void);

// void _int80Handler(void);

void _exception00Handler(void);
void _exception06Handler(void);

extern uint64_t * storeRegs();

void _cli(void);

void _sti(void);

void _hlt(void);

void picMasterMask(uint8_t mask);

void picSlaveMask(uint8_t mask);

//Termina la ejecución de la cpu.
void haltcpu(void);

#endif /* INTERRUPS_H_ */
