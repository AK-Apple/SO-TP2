#ifndef QUEUE2_H
#define QUEUE2_H

#include <stdint.h>
#include "scheduling.h"

#define MAX_SIZE2 2048

typedef struct {
    char data[MAX_SIZE2]; // Array to hold queue elements
    int front;          // Index of the front element
    int rear;           // Index of the rear element
    int size;           // Current size of the queue
} queue2_t;

// Function declarations
void init_queue2(queue2_t* q);
uint8_t is_full2(queue2_t* q);
uint8_t is_empty2(queue2_t* q);
uint8_t enqueue2(queue2_t* q, char value);
uint8_t dequeue2(queue2_t* q, char* value);
uint64_t enqueue_string2(queue2_t* q, const char* string, uint64_t size);
uint64_t dequeue_to_buffer2(queue2_t* q, char* buffer, uint64_t size);

#endif // QUEUE2_H
