#ifndef TPE_ARQUI_SOUNDS_H
#define TPE_ARQUI_SOUNDS_H

void play_song(int song_id);    // comienza una cancion (0 = silencio)
char next_part();               // devuelve 1 si la cancion termina
char next_part_instantly();     // sin timer interno

#endif //TPE_ARQUI_SOUNDS_H