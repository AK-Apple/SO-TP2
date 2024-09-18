#include <stdint.h>
#include "lib.h"
#include "video.h"
#include "IO.h"
#include "interrupts.h"
#include "naiveConsole.h"

void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

int buflen(const char *s) {
    int i = 0;
    while (*s != '\0')
    {
        i++;
        s++;
    }
    return i;
}

#define REGS_SIZE 18

uint64_t * regs = {0};
int regsCaptured = 0;

char * regList[REGS_SIZE] = {
        "RAX", "RBX", "RCX", "RDX", "RDI", "RSI","RBP", "RSP",
        "R08", "R09", "R10", "R11", "R12", "R13", "R14",
        "R15", "IP ", "RFLAGS "};

void getRegs() {
    regs = storeRegs();		// llamada a Assembler
    regsCaptured = 1;
}

void sys_registers() {
    if (!regsCaptured) {
        getRegs();
    } else {
        printf("Tenias estos registros guardados:\n");
    }

    for (int i = 0; i < REGS_SIZE ; i++) {
        char * buf;
        printf(regList[i]);
        uint32_t digits = uintToBase(regs[i], buf, 16);
        printf(" : 0x");
        int zeros = 15;
        while(zeros > digits){
            printf("0");
            zeros--;
        }
        printf(buf);
        putChar('\n');
    }
    regsCaptured = 0;
}