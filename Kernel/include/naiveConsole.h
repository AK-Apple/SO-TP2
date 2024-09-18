#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>

/* Estas no venian */
void writeCharacter(unsigned char c, unsigned char forecolour, unsigned char backcolour);
void writeStr(char * str, unsigned char forecolour, unsigned char backcolour);

/* Estas ya venian */
void ncPrint(const char * string);
void ncPrintChar(char character);
void ncNewline();
void ncPrintDec(uint64_t value);
void ncPrintHex(uint64_t value);
void ncPrintBin(uint64_t value);
void ncPrintBase(uint64_t value, uint32_t base);
void ncClear();
uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

#endif