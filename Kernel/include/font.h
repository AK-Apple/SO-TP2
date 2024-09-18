#ifndef TPE_ARQUI_FONT_H
#define TPE_ARQUI_FONT_H

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

#include <stdint.h>

void getCharMap(char c, uint8_t charMap[FONT_HEIGHT][FONT_WIDTH]);

#endif //TPE_ARQUI_FONT_H
