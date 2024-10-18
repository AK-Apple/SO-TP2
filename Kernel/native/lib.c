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
        char buf[MAX_BUF_SIZE];
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

//TODO: cambiar sys_registers() para que acepte procesos

// ------- Funciones auxiliares para el Kernel -------

int k_strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}



void k_print_int_dec(int64_t number) {
    if (number < 0) {
        putChar('-');  // Handle negative sign for negative numbers
        number = -number;  // Convert to positive for easier handling
    }

    if (number == 0) {
        putChar('0');  // Special case for 0
        return;
    }

    // Calculate and store the digits in reverse order
    char buffer[20];  // Buffer to hold the digits (enough for a 64-bit integer)
    int index = 0;

    while (number > 0) {
        buffer[index++] = (number % 10) + '0';  // Store the least significant digit as a character
        number /= 10;  // Remove the last digit
    }

    // Print the digits in the correct order
    while (index > 0) {
        putChar(buffer[--index]);  // Print the digits from most significant to least significant
    }
}

void k_print_integer(uint64_t number, uint64_t padding, uint64_t base) {
    uint64_t length = 0;
    uint64_t numberReverse = 0; 
    while(number > 0) {
        length++;
        numberReverse = numberReverse * base + number % base;
        number /= base;

    }
    for(uint64_t i = length; i < padding; i++) 
        putOut('0');
    while (length-- || numberReverse > 0) {
        int remainder = numberReverse % base;
        putOut((remainder > 9) ? (remainder - 10) + 'A' : remainder + '0');
        numberReverse /= base;
    }
}


int64_t k_atoi(const char *str) {
    int64_t result = 0;
    int sign = 1;  // Positive by default

    // Skip any leading white space characters
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r' || *str == '\f' || *str == '\v') {
        str++;
    }

    // Check if the number is negative
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Convert characters to integer
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}
