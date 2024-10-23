#include "lib.h"
#include "IO.h"

#define MAX_SIZE 32 // Maximum size of the queue

typedef struct {
    int data[MAX_SIZE]; // Array to hold queue elements
    int front;          // Index of the front element
    int rear;           // Index of the rear element
    int size;           // Current size of the queue
} queue_t;

// Initialize the queue
void init_queue(queue_t* q) {
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

// Check if the queue is full
uint8_t is_full(queue_t* q) {
    return q->size == MAX_SIZE;
}

// Check if the queue is empty
uint8_t is_empty(queue_t* q) {
    return q->size == 0;
}

// Enqueue function
uint8_t enqueue(queue_t* q, int value) {
    printf("Enqueue\n");
    if (is_full(q)) {
        printf("Queue is full!\n");
        return 0; // Indicate failure
    }
    q->rear = (q->rear + 1) % MAX_SIZE; // Circular increment
    q->data[q->rear] = value;
    q->size++;
    return 1; // Indicate success
}

// Dequeue function
uint8_t dequeue(queue_t* q, int* value) {
    if (is_empty(q)) {
        return 0; // Indicate failure
    }
    *value = q->data[q->front];
    q->front = (q->front + 1) % MAX_SIZE; // Circular increment
    q->size--;
    return 1; // Indicate success
}
