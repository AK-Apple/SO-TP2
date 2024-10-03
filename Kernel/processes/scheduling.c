#include "scheduling.h"
#include "round_robin.h"
#include "lib.h"

typedef uint64_t Stack[STACK_SIZE];
Stack stacks[MAX_PROCESS_BLOCKS] = {0};


typedef enum {
    RUNNING,  // Typically assigned 0 by default
    READY,    // Assigned 1
    BLOCKED   // Assigned 2
} State;

typedef struct ProcessBlock {
    uint64_t stack_pointer;
    State process_state;
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
            // printf("MEMORY RUN OUT!!\n");    // Por alguna razón no anda
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





// --------- scheduler --------

#define ALIGN 4


void initializeRegisters(uint64_t rip, uint64_t rsp, uint64_t stack_base){
    StackedRegisters stackedRegisters = (StackedRegisters){0};
    stackedRegisters.rflags = 0x202;
    stackedRegisters.cs = 0x8;
    stackedRegisters.rip = rip;
    stackedRegisters.rsp = rsp;

    // TODO: revisar esto, porque tengo un mal presentimiento de esto
    memcpy( (void*) stack_base, &stackedRegisters, sizeof(struct StackedRegisters));
}

uint64_t running_pid; 

CircularList round_robin = {0};

uint64_t schedule(uint64_t running_stack_pointer){
    blocks[running_pid].stack_pointer = running_stack_pointer;
    blocks[running_pid].process_state = READY;

    uint64_t next_pid = next(&round_robin);

    running_pid = next_pid;
    blocks[next_pid].process_state = RUNNING;
    return blocks[next_pid].stack_pointer;

    // TODO: ver si cambiar otras variables también
}


/// --------- end scheduling --------

/// --------- Syscalls --------

void create_init_process(){
    blocks[0].pid = 0;
    blocks[0].stack_pointer = 0;    // Se va a actualizar. El valor no importa
    blocks[0].process_state = RUNNING;
    blocks[0].parent_pid = 0;       // TODO: designarle un valor. Por ahora, no importa mucho
    blocks[0].priority = 1;

    add(&round_robin, 0);

    // OBS: el proceso INIT va a usar otro stack
}

void create_process_beta(uint64_t rip) {
    uint64_t pid = request_pid();
    uint64_t rsp = stacks[pid] + STACK_SIZE - ALIGN;      // TODO: no sé si va este - 64
    // arriba del RSP, hay que poner los valores de RAX, RBX, etc.
    initializeRegisters(rip, rsp, rsp);

    blocks[pid].pid = pid;
    blocks[pid].stack_pointer = rsp;
    blocks[pid].process_state = READY;
    blocks[pid].parent_pid = running_pid;
    blocks[pid].priority = 1;

    add(&round_robin, pid);
}

void getpid() {
    // running --> pcb --> pid
}

void info_processes() {
    // ready --> pcb
}

void get_all_processes() {
    // recorro todo y voy calculando
}


void change_priority(uint64_t pid, int value) {
    if(value == 0) return;
    if(value < 0) {
        // si prioridad > 1, saco elemento de la ronda
    }
    else {
        // si prioridad < 5, agrego elemento a la ronda
    }
}

void block() {
    // manda proceso de RUNNING a BLOCKED
}

void unlock(uint64_t pid) {
    // manda proceso de BLOCKED a READY con prioridad = 1
}

void resume() {
    // ni idea lo que hace
}

void waitpid() {
    // ni idea cómo hacer este
}

// --------- end syscalls --------
