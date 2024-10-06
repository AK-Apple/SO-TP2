#include "time.h"
#include "lib.h"
#include "naiveConsole.h"
#include "interrupts.h"
#include "scheduling.h"
#include "IO.h"

static unsigned long ticks = 0;


void timer_handler() {
	ticks++;

    // printf("A continuacion: cambio de proceso\n");
    // k_print_int_dec(getpid());

}

int ticks_elapsed() {
	return ticks;
}


int seconds_elapsed() {
	return ticks / 18;
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