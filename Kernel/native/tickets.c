#include "tickets.h"

void initialize_tickets(Tickets *tickets, void *base_address, uint64_t chunk_bytes, int64_t max_ticket) {
    tickets->base_address = base_address;
    tickets->chunk_bytes = chunk_bytes;
    tickets->max_ticket = max_ticket;
    tickets->current_available_ticket = -1;
    tickets->biggest_available_ticket = 0;
}

int64_t request_ticket(Tickets *tickets) {
    if(tickets->current_available_ticket == -1) {
        if(tickets->biggest_available_ticket >= tickets->max_ticket) {
            return -1;
        }
        return tickets->biggest_available_ticket++;
    }
    int64_t ticket = tickets->current_available_ticket;
    tickets->current_available_ticket = *(int64_t *)(tickets->base_address + tickets->chunk_bytes * ticket);
    
    return ticket;
}

void free_ticket(Tickets *tickets, int64_t ticket) {
    int64_t *ticket_address = (int64_t *)(tickets->base_address + tickets->chunk_bytes * ticket);
    *ticket_address = tickets->current_available_ticket;
    tickets->current_available_ticket = ticket;
}
