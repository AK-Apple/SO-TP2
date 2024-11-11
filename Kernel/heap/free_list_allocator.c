// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "memory_allocator.h"
#include "./../../Shared/shared.h"
#include "IO.h"
#include <stddef.h>
#include <stdint.h>
#include "lib.h"

#ifndef USE_BUDDY_ALLOCATOR
typedef struct Allocation_Header {
    uint64_t size;
    union {
        uint64_t intended_size;
        struct Allocation_Header *next;
    };
} Allocation_Header;

typedef struct Free_List_Allocator {
    void *base_address;
    uint64_t total_bytes;
    Allocation_Header *free_list;
} Free_List_Allocator;
Free_List_Allocator allocator = {0};

static void coalesce_node(Allocation_Header *node) {
    if(node == NULL) return; 
    if(node->next && (void *)((void *)node + node->size) == node->next) {
        node->size += node->next->size;
        node->next = node->next->next;
    }
}

void initialize_memory_allocator(void *base_address, uint64_t total_bytes) 
{
    total_bytes -= total_bytes % sizeof(Allocation_Header);
    base_address += sizeof(Allocation_Header) - (uint64_t)base_address % sizeof(Allocation_Header);
    allocator.base_address = base_address;
    allocator.total_bytes = total_bytes;

    Allocation_Header *first_node = (Allocation_Header *)base_address;
    first_node->size = total_bytes;
    first_node->next = NULL;
    allocator.free_list = first_node;
}
void *memory_alloc(size_t bytes) 
{
    if(bytes == 0) return NULL;
    size_t required_size = sizeof(Allocation_Header) * ((bytes - 1) / sizeof(Allocation_Header) + 2);

    Allocation_Header *list_iter = allocator.free_list;
    Allocation_Header *best_node = NULL;
    Allocation_Header *prev_node = NULL;
    Allocation_Header *best_node_prev = NULL;
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
void memory_free(void *pointer) 
{
    if(pointer == NULL) return;
    Allocation_Header *free_node = (Allocation_Header *)(pointer - sizeof(Allocation_Header));
    Allocation_Header *list_iter = allocator.free_list;
    Allocation_Header *prev_node = NULL;

    free_node->next = NULL;

    while(list_iter && (Allocation_Header *)pointer > list_iter) {
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
    coalesce_node(prev_node);
}
void memory_info(Memory_Info *info) 
{
    Allocation_Header *list_iter = allocator.free_list;
    void *current_pointer = allocator.base_address;
    uint64_t total_memory = allocator.total_bytes;
    uint64_t internal_fragmentation = 0;
    uint64_t used_memory = 0;
    uint64_t largest_free_block_bytes = 0;

    while(current_pointer - allocator.base_address < total_memory) {
        Allocation_Header *node = (Allocation_Header *)current_pointer;
        if(node == list_iter) {
            if(largest_free_block_bytes < node->size) {
                largest_free_block_bytes = node->size;
            }
            // printf_color("|%lu:free", 0x00888888, node->size);
            list_iter = list_iter->next;
            current_pointer += node->size;
        }
        else {
            // printf_color("|%lu:%lu", 0x00DDDDFF, node->size, node->intended_size);
            internal_fragmentation += node->size - node->intended_size - sizeof(Allocation_Header);
            current_pointer += node->size;
            used_memory += node->size;
        }
    }
    
    info->allocator_type = "free_list";
    info->header_size = sizeof(Allocation_Header);
    info->total_memory = allocator.total_bytes;
    info->base_address = (uint64_t)allocator.base_address;
    info->end_address = (uint64_t)allocator.base_address + total_memory - 1;
    info->largest_free_block = largest_free_block_bytes;
    info->used_memory = used_memory;
    info->free_memory = total_memory - used_memory;
    info->internal_fragmentation = internal_fragmentation;
}
#endif
