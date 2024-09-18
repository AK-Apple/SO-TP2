#include "../include/sounds.h"
#include "../include/syscalls.h"
#define MAX_SIZE 128

typedef enum {
    SILENCE, E2, F2, Fs2, G2, Gs2, A2, As2, B2, C3, Cs3, D3, Ds3, E3, F3, Fs3,
    G3, Gs3, A3, As3, B3, C4, Cs4, D4, Ds4, E4, F4, Fs4, G4, Gs4, A4, As4, B4,
    C5, Cs5, D5, Ds5, E5
} MusicalNote;

typedef struct {
    MusicalNote data[MAX_SIZE];
    int front;
    int rear;
    int size;
} BoundedQueue;

BoundedQueue not_played_yet;

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
BoundedQueue not_played_yet; // Declaration of global BoundedQueue variable

// Function to initialize the queue
void initializeQueue() {
    not_played_yet.front = 0;
    not_played_yet.rear = -1;
    not_played_yet.size = 0;
}

void initializeQueueWithArray(int arr[], int n) {
    if (n >= MAX_SIZE) return;
    not_played_yet.front = 0;
    not_played_yet.rear = n - 1; // Rear points to the last element
    not_played_yet.size = n;

    // Copy elements from the array to the queue
    for (int i = 0; i < n; i++) {
        not_played_yet.data[i] = arr[i];
    }
}

// Function to check if the queue is empty
char isEmpty() {
    return not_played_yet.size == 0 ? 1 : 0;
}

// Function to check if the queue is full
char isFull() {
    return not_played_yet.size == MAX_SIZE ? 1 : 0;
}

// Function to get the current size of the queue
int getSize() {
    return not_played_yet.size;
}

// Function to add an element to the queue
char enqueue(MusicalNote value) {
    if (isFull()) {
        return 0; // Queue is full
    }
    not_played_yet.rear = (not_played_yet.rear + 1) % MAX_SIZE;
    not_played_yet.data[not_played_yet.rear] = value;
    not_played_yet.size++;
    return 1;
}

// Function to remove an element from the queue
char dequeue(MusicalNote* value) {
    if (isEmpty()) {
        return 0; // Queue is empty
    }
    *value = not_played_yet.data[not_played_yet.front];
    not_played_yet.front = (not_played_yet.front + 1) % MAX_SIZE;
    not_played_yet.size--;
    return 1;
}






// ---------------------------------------------------------------------------------



void play_song(int song_id){
    switch(song_id){
        case 1: initializeQueueWithArray(retro_song, RETRO_SONG_LENGHT); break;
        case 2: initializeQueueWithArray(os_initiating, OS_INITIALIZING_LENGTH); break;
        case 3: initializeQueueWithArray(you_lost, YOU_LOST_LENGTH); break;
        default: initializeQueue(); break;
        last_note_ticks = sys_ticks_elapsed;
    }
}

void playNote(MusicalNote note){
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

char next_part(){ // devuelve 0 si la cancion termina
    int current_ticks = sys_ticks_elapsed();

    if (current_ticks-last_note_ticks > 3) {
        MusicalNote note_to_play;
        if (!dequeue(&note_to_play)){
            sys_nosound();
            return 0;
        }
        sys_nosound();
        playNote(note_to_play);
        last_note_ticks = current_ticks;
        return 1;
    }
    return 1;
}      

char next_part_instantly(){
    MusicalNote note_to_play;
    if (!dequeue(&note_to_play)){
        sys_nosound();
        return 0;
    }
    sys_nosound();
    playNote(note_to_play);
    return 1;
}