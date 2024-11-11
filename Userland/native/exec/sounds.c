// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/sounds.h"
#include "../include/syscalls.h"
#define MAX_SIZE 128

typedef enum {
    SILENCE, E2, F2, Fs2, G2, Gs2, A2, As2, B2, C3, Cs3, D3, Ds3, E3, F3, Fs3,
    G3, Gs3, A3, As3, B3, C4, Cs4, D4, Ds4, E4, F4, Fs4, G4, Gs4, A4, As4, B4,
    C5, Cs5, D5, Ds5, E5
} MusicalNote;


int last_note_ticks = 0;

#define RETRO_SONG_LENGHT 64
MusicalNote retro_song[RETRO_SONG_LENGHT] = {           // song id 1      
    SILENCE, E3, Fs3, G3, G3, E3, SILENCE, B3,
    As3, SILENCE, As3, Fs3, SILENCE, Fs3, E3, B2,
    E2, E3, Fs3, G3, G3, E3, SILENCE, B3,
    As3, SILENCE, As3, Cs4, SILENCE, Fs3, E3, B2,
    E2, C4, B3, A3, E3, C4, SILENCE, C4,
    B3, E4, B3, E3, SILENCE, E3, G3, B3,
    G4, E4, B3, A3, G3, Fs3, Ds3, B2,
    Ds3, E3, SILENCE, E3, SILENCE, E3, SILENCE, E3
};

#define OS_INITIALIZING_LENGTH 7
MusicalNote os_initiating[OS_INITIALIZING_LENGTH] = {        // song id 2
    G4, SILENCE, G4, SILENCE, G4, E4, C4
};
#define YOU_LOST_LENGTH 5
MusicalNote you_lost[] = {             // song id 3  
    Gs4, G4, F4, E4, SILENCE
};


// ---------------------------------------------------------------------------------


void playNote(MusicalNote note)
{
    switch (note){
        case SILENCE: sys_nosound(); break;
        case E2: sys_sound(82); break;
        case F2: sys_sound(87); break;
        case Fs2: sys_sound(92); break;
        case G2: sys_sound(98); break;
        case Gs2: sys_sound(103); break;
        case A2: sys_sound(110); break;
        case As2: sys_sound(116); break;
        case B2: sys_sound(123); break;
        case C3: sys_sound(130); break;
        case Cs3: sys_sound(138); break;
        case D3: sys_sound(146); break;
        case Ds3: sys_sound(155); break;
        case E3: sys_sound(164); break;
        case F3: sys_sound(174); break;
        case Fs3: sys_sound(185); break;
        case G3: sys_sound(196); break;
        case Gs3: sys_sound(207); break;
        case A3: sys_sound(220); break;
        case As3: sys_sound(233); break;
        case B3: sys_sound(246); break;
        case C4: sys_sound(261); break;
        case Cs4: sys_sound(277); break;
        case D4: sys_sound(293); break;
        case Ds4: sys_sound(311); break;
        case E4: sys_sound(329); break;
        case F4: sys_sound(349); break;
        case Fs4: sys_sound(370); break;
        case G4: sys_sound(392); break;
        case Gs4: sys_sound(415); break;
        case A4: sys_sound(440); break;
        case As4: sys_sound(466); break;
        case B4: sys_sound(493); break;
        case C5: sys_sound(523); break;
        case Cs5: sys_sound(554); break;
        case D5: sys_sound(587); break;
        case Ds5: sys_sound(622); break;
        case E5: sys_sound(659); break;
    }
}


void play_song_continuous(int song_id)
{
    int song_length = 0;
    MusicalNote* song;
    switch(song_id){
        case 1: 
        {
            song = retro_song; 
            song_length = RETRO_SONG_LENGHT; 
            break;
        }
        case 2:
        {
            song = os_initiating; 
            song_length = OS_INITIALIZING_LENGTH; 
            break;
        }
        case 3: 
        {
            song = you_lost; 
            song_length = YOU_LOST_LENGTH; 
            break;
        }
        default:
        {
            return;
        }
    }
    for (int i=0; i < song_length; i++)
    {
        playNote(song[i]);
        sys_ticks_sleep(5);
        sys_nosound();
    }
    
}
