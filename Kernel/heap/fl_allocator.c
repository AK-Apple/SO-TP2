#include "memory_allocator.h"
#include "IO.h"
#include <stddef.h>

#ifndef USE_BUDDY_ALLOCATOR
// typedef struct FreeListMemoryManager {
//     int current_free_index;
//     void *base_adress;
//     void *free_pointers[BLOCK_COUNT];
// } FreeListMemoryManager;

// static FreeListMemoryManager fl_memory_manager = {0};

// void fl_print_mem() {
//     printf("[FL_MEM] falta implementar print mem");
// }

// int fl_total_memory() {
//     return BLOCK_COUNT * BLOCK_SIZE;
// }

// void initialize_allocator(void* heap) {
//     fl_memory_manager.base_adress = heap;
//     for(uint32_t i = 0; i < BLOCK_COUNT; i++) {
//         fl_memory_manager.free_pointers[i] = fl_memory_manager.base_adress + i * BLOCK_SIZE;
//     }
// }

// void *fl_malloc() {
//     if(fl_memory_manager.current_free_index < BLOCK_COUNT) {
//         return fl_memory_manager.free_pointers[fl_memory_manager.current_free_index++];
//     }
//     else {
//         return NULL;
//     }
// }

// void fl_free(void *pointer) {
//     fl_memory_manager.free_pointers[--fl_memory_manager.current_free_index] = pointer;
// }
#endif

