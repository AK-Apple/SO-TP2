// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "memory_allocator.h"
#include "./../../Shared/shared.h"
#include "IO.h"
#include <stddef.h>
#include <stdint.h>
#include "lib.h"

#ifndef USE_BUDDY_ALLOCATOR
typedef struct AllocationHeader {
    uint64_t size;
    union {
        uint64_t intended_size;
        struct AllocationHeader *next;
    };
} AllocationHeader;

typedef struct FreeListAllocator {
    void *base_address;
    uint64_t total_bytes;
    AllocationHeader *free_list;
} FreeListAllocator;
FreeListAllocator allocator = {0};

static void coalesce_node(AllocationHeader *node) {
    if(node == NULL) return; 
    if(node->next && (void *)((void *)node + node->size) == node->next) {
        node->size += node->next->size;
        node->next = node->next->next;
    }
}

void initialize_memory_allocator(void *base_address, uint64_t total_bytes) {
    total_bytes -= total_bytes % sizeof(AllocationHeader);
    base_address += sizeof(AllocationHeader) - (uint64_t)base_address % sizeof(AllocationHeader);
    allocator.base_address = base_address;
    allocator.total_bytes = total_bytes;

    AllocationHeader *first_node = (AllocationHeader *)base_address;
    first_node->size = total_bytes;
    first_node->next = NULL;
    allocator.free_list = first_node;
}
void *memory_alloc(size_t bytes) {
    if(bytes == 0) return NULL;
    size_t required_size = sizeof(AllocationHeader) * ((bytes - 1) / sizeof(AllocationHeader) + 2);

    AllocationHeader *list_iter = allocator.free_list;
    AllocationHeader *best_node = NULL;
    AllocationHeader *prev_node = NULL;
    AllocationHeader *best_node_prev = NULL;
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
    if(remaining_size == sizeof(AllocationHeader)) {
        required_size += sizeof(AllocationHeader);
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

    AllocationHeader *header = (AllocationHeader *)((uint64_t)best_node + remaining_size);
    header->size = required_size;
    header->intended_size = bytes;

    return (void *)header + sizeof(AllocationHeader);
}
void memory_free(void *pointer) {
    if(pointer == NULL) return;
    AllocationHeader *free_node = (AllocationHeader *)(pointer - sizeof(AllocationHeader));
    AllocationHeader *list_iter = allocator.free_list;
    AllocationHeader *prev_node = NULL;

    free_node->next = NULL;

    while(list_iter && (AllocationHeader *)pointer > list_iter) {
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
void memory_info(Memory_Info *info) {
    AllocationHeader *list_iter = allocator.free_list;
    void *current_pointer = allocator.base_address;
    uint64_t total_memory = allocator.total_bytes;
    uint64_t internal_fragmentation = 0;
    uint64_t used_memory = 0;
    uint64_t largest_free_block_bytes = 0;

    while(current_pointer - allocator.base_address < total_memory) {
        AllocationHeader *node = (AllocationHeader *)current_pointer;
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
            internal_fragmentation += node->size - node->intended_size - sizeof(AllocationHeader);
            current_pointer += node->size;
            used_memory += node->size;
        }
    }
    
    info->allocator_type = "free_list";
    info->header_size = sizeof(AllocationHeader);
    info->total_memory = allocator.total_bytes;
    info->base_address = (uint64_t)allocator.base_address;
    info->end_address = (uint64_t)allocator.base_address + total_memory - 1;
    info->largest_free_block = largest_free_block_bytes;
    info->used_memory = used_memory;
    info->free_memory = total_memory - used_memory;
    info->internal_fragmentation = internal_fragmentation;
}
#endif
