#ifndef TPE_ARQUI_VIDEO_H
#define TPE_ARQUI_VIDEO_H

#include <stdint.h>

#define OUT_FORE_COLOR 0x00ffffff
#define ERR_FORE_COLOR 0x00ff0000
#define BG_COLOR 0x00000000

void put_pixel(uint32_t hex_color, uint64_t x, uint64_t y);
void put_square(uint32_t hex_color, uint64_t x, uint64_t y, uint64_t size);
int get_width();
int get_height();

// 0 --> getWidth   (o cualquier cosa que no sea 1)
// 1 --> getHeight
int sys_get_size(int id);

void clear_screen(uint32_t hex_color);





#endif //TPE_ARQUI_VIDEO_H
