#include "memory_allocator.h"
#include "IO.h"
#include <stddef.h>
#include <stdint.h>

#ifndef USE_BUDDY_ALLOCATOR
typedef struct Allocation_Header {
    uint64_t size;
    uint64_t intended_size;
} Allocation_Header;

typedef struct Free_List_Node {
    uint64_t size;
    struct Free_List_Node *next;
} Free_List_Node;

typedef struct Free_List_Allocator {
    void *base_address;
    uint64_t total_bytes;
    Free_List_Node *free_list;
} Free_List_Allocator;
Free_List_Allocator allocator = {0};

static void coalesce_node(Free_List_Node *node) {
    if(node->next && (void *)((void *)node + node->size) == node->next) {
        node->size += node->next->size;
        if(node)
            node->next = node->next->next;
        else    
            allocator.free_list = node->next->next;
    }
}

void initialize_memory_allocator(void *base_address, uint64_t total_bytes) {
    allocator.base_address = base_address;
    allocator.total_bytes = total_bytes;

    Free_List_Node *first_node = (Free_List_Node *)base_address;
    first_node->size = total_bytes;
    first_node->next = NULL;
    allocator.free_list = first_node;
}
void *memory_alloc(size_t bytes) {
    if(bytes == 0) return NULL;
    size_t required_size = sizeof(Allocation_Header) * ((bytes - 1) / sizeof(Allocation_Header) + 2);

    Free_List_Node *list_iter = allocator.free_list;
    Free_List_Node *best_node = NULL;
    Free_List_Node *prev_node = NULL;
    Free_List_Node *best_node_prev = NULL;
    while(list_iter) {
        if(required_size <= list_iter->size && (!best_node || list_iter->size < best_node->size)) {
            best_node = list_iter;
            best_node_prev = prev_node;
        }
        prev_node = list_iter;
        list_iter = list_iter->next;
    }

    if(best_node == NULL) return NULL;

    uint64_t remaining_size = best_node->size - required_size;
    if(remaining_size == sizeof(Allocation_Header)) {
        required_size += sizeof(Allocation_Header);
        remaining_size = 0;
    }
    if(remaining_size > 0) {
        best_node->size = remaining_size;
    }
    else if(best_node_prev) {
        best_node_prev->next = best_node->next;
    }
    else {
        allocator.free_list = best_node->next;
    }

    Allocation_Header *header = (Allocation_Header *)((uint64_t)best_node + remaining_size);
    header->size = required_size;
    header->intended_size = bytes;

    return (void *)header + sizeof(Allocation_Header);
}
void memory_free(void *pointer) {
    if(pointer == NULL) return;
    Allocation_Header *header = (Allocation_Header *)(pointer - sizeof(Allocation_Header));
    Free_List_Node *list_iter = allocator.free_list;
    Free_List_Node *prev_node = NULL;
    Free_List_Node *free_node = (Free_List_Node *)header;

    free_node->size = header->size; // si estan en el mismo orden en el struct, entonces esto no hace nada
    free_node->next = NULL;

    while(list_iter && (Free_List_Node *)pointer > list_iter) {
        prev_node = list_iter;
        list_iter = list_iter->next;
    }
    if(prev_node) {
        free_node->next = prev_node->next;
        prev_node->next = free_node;
    }
    else {
        free_node->next = allocator.free_list;
        allocator.free_list = free_node;
    }
    coalesce_node(free_node);
    if(prev_node)
        coalesce_node(prev_node);
}
void memory_info() {
    Free_List_Node *list_iter = allocator.free_list;
    void *current_pointer = allocator.base_address;
    uint64_t total_memory = allocator.total_bytes;
    uint64_t internal_fragmentation = 0;
    uint64_t used_memory = 0;
    
    while(current_pointer - allocator.base_address < total_memory) {
        if((Free_List_Node *)current_pointer == list_iter) {
            Free_List_Node *node = (Free_List_Node *)current_pointer;
            set_foreground_color(0x00888888);
            printf("|%d:free", node->size);
            set_foreground_color(0x00FFFFFF);
            list_iter = list_iter->next;
            current_pointer += node->size;
        }
        else {
            Allocation_Header *header = (Allocation_Header *)current_pointer;
            printf("|%d:%d", header->size, header->intended_size);
            internal_fragmentation += header->size - header->intended_size - sizeof(Allocation_Header);
            current_pointer += header->size;
            used_memory += header->size;
        }
    }
    printf("\nTotal memory: %d bytes\n", total_memory);
    printf("Used memory %d bytes\n", used_memory);
    printf("Free memory %d bytes\n", total_memory - used_memory);
    printf("internal fragmentation %d bytes\n", internal_fragmentation);
    printf("largest free block %d bytes\n", largest_free_block());

}
size_t largest_free_block() {
    Free_List_Node *list_iter = allocator.free_list;
    uint64_t biggest_size = 0;
    while(list_iter) {
        if(biggest_size < list_iter->size)
            biggest_size = list_iter->size;

        list_iter = list_iter->next;
    }
    return biggest_size;
}
#endif

