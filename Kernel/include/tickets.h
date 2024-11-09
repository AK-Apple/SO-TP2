#ifndef TICKETS_SYSTEM_H
#define TICKETS_SYSTEM_H

#include <stdint.h>
#include <stddef.h>

/*
funciona similar a un memory allocator que retorna bloques del mismo size
se almacena una free list como un linked list dentro de la memoria base_address, para saber el siguiente nodo se usa su indice.
Tickets se usa para los pid_t y pipeid que necesitan allocs arbitrarios y frees arbitrarios, todos con el mismo size
*/
typedef struct {
    void *base_address;
    uint64_t chunk_bytes;
    int64_t max_ticket;
    int64_t current_available_ticket;
    int64_t biggest_available_ticket;
} Tickets;

void initialize_tickets(Tickets *tickets, void *base_address, uint64_t chunk_bytes, int64_t max_ticket);

int64_t request_ticket(Tickets *tickets);

void free_ticket(Tickets *tickets, int64_t ticket);

#endif