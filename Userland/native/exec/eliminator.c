#include "../include/command.h"
#include "../include/shell.h"
#include "../include/stdlib.h"
#include "../include/eliminator.h"
#include "../include/syscalls.h"
#include "../include/stdio.h"
#include "../include/sounds.h"

#define DELAY 3
#define SQUARE_SIZE 35

static int width = 950;   //declaro por las dudas
static int height = 700;
int ticks_delay = 3;

typedef struct {
    int x, y;
} Segment;

typedef struct {
    int x, y, dir;
    Segment tail[100000];
    int tail_length;
} Player;

Player player;
int game_over = 0;

int adjust(int coords){
    return (coords/SQUARE_SIZE)*SQUARE_SIZE;
}

void init() {
    width = sys_getWindowSize(0);
    height = sys_getWindowSize(1);
    player.x = adjust(width / 2);
    player.y = adjust(height / 2);
    player.dir = 'd';  // Initialize direction to right
    player.tail_length = 0;
}

void end_game() {
    game_over = 1;
}

void add_tail_segment(int x, int y) {
    player.tail[player.tail_length].x = x;
    player.tail[player.tail_length].y = y;
    player.tail_length++;
}

void move_player() {
    int new_x = player.x;
    int new_y = player.y;

    switch (player.dir) {
        case 'w': new_y-= SQUARE_SIZE; break;
        case 's': new_y+= SQUARE_SIZE; break;
        case 'a': new_x-= SQUARE_SIZE; break;
        case 'd': new_x+= SQUARE_SIZE; break;
    }

    // Check for collisions with wall
    if (new_x < SQUARE_SIZE || new_x >= width - 2*SQUARE_SIZE || new_y < SQUARE_SIZE || new_y >= height - 2*SQUARE_SIZE) {
        end_game();
    }

    // Check for collisions with tail
    for (int i = 0; i < player.tail_length; i++) {
        if (player.tail[i].x == new_x && player.tail[i].y == new_y) {
            end_game();
        }
    }

    // Move the tail
    if (player.tail_length > 0) {
        for (int i = player.tail_length - 1; i > 0; i--) {
            player.tail[i] = player.tail[i - 1];
        }
        player.tail[0].x = player.x;
        player.tail[0].y = player.y;
    }
    
    add_tail_segment(player.x, player.y);
    player.x = new_x;
    player.y = new_y;
}

void draw() {
    putSquare(0xFFFF00, player.x, player.y, SQUARE_SIZE);  // Draw player as yellow square
    // Draw tail
    putSquare(0x00FF00, player.tail[player.tail_length-1].x, player.tail[player.tail_length-1].y, SQUARE_SIZE);  // Draw tail as green squares
}

void handle_input() {
    // Simple input handling (replace with appropriate input mechanism)
    // This is just a placeholder to demonstrate the logic.
    char ch = getKey();
    switch (ch) {
        case 'w':
        case 's':
        case 'a':
        case 'd':
            player.dir = ch;
            break;
        case 'q':
            end_game();
            break;
    }
}

void delay(int target_ticks) {
    int start_ticks = sys_ticks_elapsed();
    while (sys_ticks_elapsed() - start_ticks < target_ticks) {
        // Busy-wait loop
    }
}

void printMenu() {

    printf("Welcome to THE GAME\nPress a number from 1 to 5 for difficulty\n");
    char difficulty;
    char selected = 0;
    do {
        difficulty = getchar();
        switch (difficulty) {
            case '1': ticks_delay = 10; selected = 1; break;
            case '2': ticks_delay = 8; selected = 1; break;
            case '3': ticks_delay = 6; selected = 1; break;
            case '4': ticks_delay = 4; selected = 1; break;
            case '5': ticks_delay = 2; selected = 1; break;
        }
    } while (!selected);

}

void gameOver(int score) {
    printf_color("\n\tGame Over!\n", 0xFF0000, 0x000000);
    printf("\tYour score was: ");
    printInt(score);
    printf("\n");
    printf("\n Press q to quit the game, press w to try again\n");
}

void handleScore(int startingPoint, int lastScore) {
    int score = sys_secondsElapsed()-startingPoint;
    if (score > lastScore) {
        for(int i=0; i<12; i++){     // borra el score anterior
            putchar(0x08);
        }
        printf("score: ");
        printInt(score);            // pone el nuevo score
        lastScore = score;
    }
}

void printBorders(void) {
    for(int i=0; i<width; i+=SQUARE_SIZE){
        putSquare(0xFF0000, i, 0, SQUARE_SIZE); 
        putSquare(0xFF0000, i, adjust(height)-SQUARE_SIZE, SQUARE_SIZE); 
        putSquare(0xFF0000, i, adjust(height), SQUARE_SIZE); 

    }
    for(int j=0; j<height; j+=SQUARE_SIZE){
        putSquare(0xFF0000, 0, j, SQUARE_SIZE); 
        putSquare(0xFF0000, adjust(width)-SQUARE_SIZE, j, SQUARE_SIZE); 
        putSquare(0xFF0000, adjust(width), j, SQUARE_SIZE); 
    }
}

void eliminator() {
    char quit_game =0;
    while (!quit_game){
        sys_clear();
        printMenu();
        sys_clear();
        init();

        int startingPoint = sys_secondsElapsed();
        int lastScore = 0;
        printBorders();
        while (!game_over) {
            handleScore(startingPoint, lastScore);
            handle_input();
            move_player();
            draw();
            if (!next_part_instantly()){
                play_song(1);       // suena "RETRO_SONG"
            }
            delay(ticks_delay);
        }
        play_song(3);               // suena "YOU_LOST"
        while (next_part());

        game_over = 0;

        int endingPoint = sys_secondsElapsed();
        int score = endingPoint - startingPoint;
        sys_clear();

        gameOver(score);

        char selected = 0;
        char input;
        do {
            input = getchar();
            switch (input) {
                case 'q': quit_game = 1 ; selected = 1; break;
                case 'w': selected = 1 ; break;
            }
        } while (!selected);
    }
    sys_clear();
    return;
}