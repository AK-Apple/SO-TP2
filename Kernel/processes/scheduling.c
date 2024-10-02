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
uint64_t current_available_pid_index = 0;
uint64_t biggest_pid = 0;

uint64_t request_pid() {
    if(current_available_pid_index >= biggest_pid) {
        if(biggest_pid >= MAX_PROCESS_BLOCKS) {
            printf("MEMORY RUN OUT!!\n");
            return INVALID_PID;
        }
        biggest_pid++;
        return current_available_pid_index++;
    }
    return available_pids[current_available_pid_index++];
}

void free_pid(int pid) {
    available_pids[--current_available_pid_index] = pid;
}


ProcessBlock blocks[MAX_PROCESS_BLOCKS] = {0};
//TODO: hacer que tire error cuando pid > 64



void create_process(uint64_t parent_pid, char **argv) {

}

void kill_process(uint64_t pid) {
    
}

void store_context() {
    
}



