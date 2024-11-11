#ifndef PQUEUE_H
#define PQUEUE_H

#include <stdint.h>
#include "process.h"

#define IO_BUF_SIZE 64

typedef struct {
    char data[IO_BUF_SIZE]; // Array to hold queue elements
    int front;          // Index of the front element
    int rear;           // Index of the rear element
    int size;           // Current size of the queue
} pqueue_t;

// pqueue = queue for pipes

void init_pqueue(pqueue_t* q);
uint8_t p_is_full(pqueue_t* q);
uint8_t p_is_empty(pqueue_t* q);
uint8_t p_enqueue(pqueue_t* q, char value);
uint8_t p_dequeue(pqueue_t* q, char* value);

#endif // PQUEUE_H
