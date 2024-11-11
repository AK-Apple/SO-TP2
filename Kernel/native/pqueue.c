// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "lib.h"
#include "pqueue.h"
#include "IO.h"

void init_pqueue(pqueue_t* q) 
{
    q->front = 0;
    q->rear = 0;
    q->size = 0;
    memset(q->data, 0, IO_BUF_SIZE);
}

uint8_t p_is_full(pqueue_t* q) 
{
    return q->size == IO_BUF_SIZE;
}

uint8_t p_is_empty(pqueue_t* q) 
{
    return q->size == 0;
}

uint8_t p_enqueue(pqueue_t* q, char value) 
{
    if (p_is_full(q)) 
    {
        return 0;
    }
    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % IO_BUF_SIZE;

    q->size++;
    return 1;
}


// Dequeue function
uint8_t p_dequeue(pqueue_t* q, char* value) 
{
    if (p_is_empty(q)) 
    {
        return 0; // Indicate failure
    }
    *value = q->data[q->front];
    q->front = (q->front + 1) % IO_BUF_SIZE; // Circular increment
    q->size--;
    return 1; // Indicate success
}
