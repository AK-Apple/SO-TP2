#ifndef TPE_SOUND_H
#define TPE_SOUND_H

#include <stdint.h>
#include "shared.h"

void play_sound(uint32_t nFrequence);
void nosound();
pid_t get_player();

#endif //TPE_SOUND_H
