#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include "process.h"

#define MAX_SIZE MAX_PROCESS_BLOCKS

typedef struct {
    int data[MAX_SIZE]; // Array to hold queue elements
    int front;          // Index of the front element
    int rear;           // Index of the rear element
    int size;           // Current size of the queue
} queue_t;

// Function declarations
void init_queue(queue_t* q);
uint8_t is_full(queue_t* q);
uint8_t is_empty(queue_t* q);
uint8_t enqueue(queue_t* q, int value);
uint8_t dequeue(queue_t* q, int64_t* value);

#endif // QUEUE_H
