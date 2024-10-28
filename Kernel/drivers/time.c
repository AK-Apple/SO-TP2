// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "time.h"
#include "lib.h"
#include "interrupts.h"
#include "scheduling.h"
#include "IO.h"

uint64_t ticks = 0;


void timer_handler() {
	ticks++;
}

uint64_t ticks_elapsed() {
	return ticks;
}


uint64_t seconds_elapsed() {
	return ticks / 18;
}

void ticks_sleep(uint64_t ticks_to_sleep)
{

    uint64_t started_at = ticks;
    while (ticks_elapsed() < started_at + ticks_to_sleep) 
        _hlt();

    return;
}

void seconds_sleep(uint64_t seconds_to_sleep)
{
    uint64_t started_at = seconds_elapsed();
    while (seconds_elapsed() < started_at + seconds_to_sleep) 
        _hlt();

    return;
}


uint8_t getFormat(uint8_t num) {
    int dec = num & 240;
    dec = dec >> 4;
    int units = num & 15;
    return dec * 10 + units;
}

uint8_t sys_getTime(int descriptor) {
    uint8_t toReturn = getRTC(descriptor);
    return getFormat(toReturn);
}