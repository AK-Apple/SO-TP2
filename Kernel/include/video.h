#ifndef TPE_ARQUI_VIDEO_H
#define TPE_ARQUI_VIDEO_H

#include <stdint.h>

#define OUT_FORE_COLOR 0x00ffffff
#define ERR_FORE_COLOR 0x00ff0000
#define BG_COLOR 0x00000000

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
void putSquare(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t size);
int getWidth();
int getHeight();

// 0 --> getWidth   (o cualquier cosa que no sea 1)
// 1 --> getHeight
int sys_getSize(int id);

void clearScreen(uint32_t hexColor);





#endif //TPE_ARQUI_VIDEO_H
