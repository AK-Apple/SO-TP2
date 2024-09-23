#include "fl_allocator.h"
#include <stddef.h>

typedef struct FreeListMemoryManager {
    int current_free_index;
    void *base_adress;
    void *free_pointers[BLOCK_COUNT];
} FreeListMemoryManager;

static FreeListMemoryManager fl_memory_manager = {0};
// static char managed_memory[TOTAL_MEMORY_BYTES] = {0};
#define MEMEM (void*)0x500000
void initialize_allocator() {
    fl_memory_manager.base_adress = MEMEM;
    for(uint32_t i = 0; i < BLOCK_COUNT; i++) {
        fl_memory_manager.free_pointers[i] = fl_memory_manager.base_adress + i * BLOCK_SIZE;
    }
}

void *fl_malloc() {
    if(fl_memory_manager.current_free_index < BLOCK_COUNT) {
        return fl_memory_manager.free_pointers[fl_memory_manager.current_free_index++];
    }
    else {
        return NULL;
    }
}

void fl_free(void *pointer) {
    fl_memory_manager.free_pointers[--fl_memory_manager.current_free_index] = pointer;
}
