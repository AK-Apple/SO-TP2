#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

#define TICKS_TO_SECONDS(x) ((x) * 5 / 91)

void timer_handler();
int ticks_elapsed();
int seconds_elapsed();

#define SECONDS 00
#define MINUTES 02
#define HOURS 04
#define DAY_OF_THE_WEEK 06
#define DAY_OF_THE_MONTH 07
#define MONTH 08
#define YEAR 09

extern uint8_t getRTC(uint64_t descriptor); // de libasm.asm
uint8_t sys_getTime(int descriptor);

#endif
