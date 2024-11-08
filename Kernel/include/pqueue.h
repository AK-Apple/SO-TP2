#ifndef PQUEUE_H
#define PQUEUE_H

#include <stdint.h>
#include "process.h"

#define MAX_SIZE2 2048

typedef struct {
    char data[MAX_SIZE2]; // Array to hold queue elements
    int front;          // Index of the front element
    int rear;           // Index of the rear element
    int size;           // Current size of the queue
} pqueue_t;
// pqueue = queue for pipes

// Function declarations
void init_pqueue(pqueue_t* q);
uint8_t p_is_full(pqueue_t* q);
uint8_t p_is_empty(pqueue_t* q);
uint8_t p_enqueue(pqueue_t* q, char value);
uint8_t p_dequeue(pqueue_t* q, char* value);
uint64_t p_enqueue_string(pqueue_t* q, const char* string, uint64_t size);
uint64_t p_dequeue_to_buffer(pqueue_t* q, char* buffer, uint64_t size);

#endif // PQUEUE_H
