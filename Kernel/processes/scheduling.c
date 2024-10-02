#include "scheduling.h"

typedef uint64_t Stack[STACK_SIZE];
Stack stacks[MAX_PROCESS_BLOCKS] = {0};

typedef struct ProcessBlock {
    uint64_t stack_pointer;
    uint64_t process_state;
    uint64_t pid; // <---- stacks[pid] es stack_base ; blocks[i] determina i=pid 
    uint64_t parent_pid;
    uint64_t priority;
    // char **argv; 
} ProcessBlock;

uint64_t available_pids[MAX_PROCESS_BLOCKS] = {0};
uint64_t biggest_pid = 0;

uint64_t request_pid() {
    
}


ProcessBlock blocks[MAX_PROCESS_BLOCKS] = {0};
//TODO: hacer que tire error cuando pid > 64



void create_process(uint64_t parent_pid, char **argv) {

}

void kill_process(uint64_t pid) {
    
}

void store_context() {
    
}



