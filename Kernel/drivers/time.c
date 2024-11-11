// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "time.h"
#include "lib.h"
#include "interrupts.h"
#include "process.h"
#include "IO.h"

uint8_t dont_handle_ticks_flag = 0;

uint64_t ticks = 0;

void dont_handle_ticks()
{
    dont_handle_ticks_flag ++;
}


void timer_handler() 
{
    if(dont_handle_ticks_flag)
    {
        dont_handle_ticks_flag --;
        return;
    }
    else
    {
        ticks++;
    }
}

uint64_t ticks_elapsed() 
{
	return ticks;
}


uint64_t seconds_elapsed() 
{
	return ticks / TICKS_PER_SECOND;
}


uint8_t get_format(uint8_t num) {
    int dec = num & 240;
    dec = dec >> 4;
    int units = num & 15;
    return dec * 10 + units;
}

uint8_t sys_get_time(int descriptor) {
    uint8_t toReturn = getRTC(descriptor);
    return get_format(toReturn);
}