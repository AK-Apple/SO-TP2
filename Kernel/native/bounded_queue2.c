// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "lib.h"
#include "bounded_queue2.h"
#include "IO.h"

// Initialize the queue
void init_queue2(queue2_t* q) {
    q->front = 0;
    q->rear = 0;
    q->size = 0;
    memset(q->data, 0, MAX_SIZE2);
}

// Check if the queue is full
uint8_t is_full2(queue2_t* q) {
    return q->size == MAX_SIZE2;
}

// Check if the queue is empty
uint8_t is_empty2(queue2_t* q) {
    return q->size == 0;
}

// Enqueue function
uint8_t enqueue2(queue2_t* q, char value) {
    if (is_full2(q)) {
        printf_error("Queue is full!\n");
        return 0; // Indicate failure
    }
    q->rear = (q->rear + 1) % MAX_SIZE2; // Circular increment
    q->data[q->rear] = value;
    q->size++;
    return 1; // Indicate success
}

uint64_t enqueue_string2(queue2_t* q, const char* string, uint64_t size) {

    if (MAX_SIZE2 - q->size < size) {
        size = MAX_SIZE2 - q->size;
    }

    // Calculate the available space in the queue
    int available_space = MAX_SIZE2 - q->rear;

    // Copy to the end of the current data segment
    if (size <= available_space) {
        memcpy(&q->data[q->rear], string, size);
    } else {
        // Copy to the end of the data segment
        memcpy(&q->data[q->rear], string, available_space);
        // Copy the remaining part to the beginning of the array
        memcpy(q->data, string + available_space, size - available_space);
    }

    // Update the rear index and size of the queue
    q->rear = (q->rear + size) % MAX_SIZE2;
    q->size += size;

    return size;
}

// Dequeue function
uint8_t dequeue2(queue2_t* q, char* value) {
    if (is_empty2(q)) {
        return 0; // Indicate failure
    }
    *value = q->data[q->front];
    q->front = (q->front + 1) % MAX_SIZE2; // Circular increment
    q->size--;
    return 1; // Indicate success
}

uint64_t dequeue_to_buffer2(queue2_t* q, char* buffer, uint64_t size) {
    // Check if the queue is empty
    if (q->size == 0 || q->size < size) {
        return 0;  // Failure
    }


    // Store the size of the data to be removed
    uint64_t to_return_size = size;

    // Determine how much can be copied in one go
    int available_space = MAX_SIZE2 - q->front;

    // Copy data from the queue to the buffer
    if (to_return_size <= available_space) {
        memcpy(buffer, &q->data[q->front], to_return_size);
    } else {
        // Copy to the buffer from the front to the end of the data segment
        memcpy(buffer, &q->data[q->front], available_space);
        // Copy the remaining part to the buffer
        memcpy(buffer + available_space, q->data, to_return_size - available_space);
    }

    // Clear the queue by resetting its state
    q->front = (q->front + to_return_size) % MAX_SIZE2;
    q->size -= to_return_size;

    return to_return_size; // Success
}
