#include "scheduling.h"
#include "round_robin.h"
#include "lib.h"
#include "IO.h"
#include "process.h"
#include "interrupts.h"

typedef uint64_t Stack[STACK_SIZE];
Stack stacks[MAX_PROCESS_BLOCKS] = {0};


typedef enum {
    UNAVAILABLE,    // Assigned 0 by default
    RUNNING,
    READY,
    BLOCKED
} State;

typedef struct ProcessBlock {
    uint64_t stack_pointer;
    State process_state;
    uint64_t pid; // <---- stacks[pid] es stack_base ; blocks[i] determina i=pid 
    uint64_t parent_pid;
    uint64_t priority;
    char* p_name;
    int argc;
    char** argv;
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




void kill_process(uint64_t pid) {
    
}

void store_context() {
    
}





// --------- scheduler --------



uint64_t running_pid; 

CircularList round_robin = {0};

uint64_t schedule(uint64_t running_stack_pointer){
    // printf("llego al schedule\n");

    // k_print_int_dec(running_stack_pointer);

    blocks[running_pid].stack_pointer = running_stack_pointer;
    blocks[running_pid].process_state = READY;

    uint64_t next_pid = next(&round_robin);

    running_pid = next_pid;

    // k_print_int_dec(getpid());
    // k_print_int_dec(blocks[next_pid].stack_pointer);

    blocks[next_pid].process_state = RUNNING;
    return blocks[next_pid].stack_pointer;

    // TODO: ver si cambiar otras variables también
}


/// --------- end scheduling --------

// ------- Principio de un proceso --------

#define ALIGN 8

uint64_t default_rip = 0;

void initializer(){
    default_rip = _get_starting_point();
    // a partir de acá empieza un proceso
    printf("Empezo el proceso nro ");
    k_print_int_dec(running_pid); putChar('\n');

    char* name = blocks[running_pid].p_name;

    putChar('a');

    int argc = blocks[running_pid].argc;
    char** argv = blocks[running_pid].argv;
    printf(blocks[running_pid].p_name); putChar('\n'); putChar('a');

    if (name == 0) return;   // para el proceso init
    uint64_t return_value = process_initializer(name, argc, argv);
    // exit(return_value);

    // Cosas que pueden estar mal:
    // 1. El orden del struct de StackedRegisters
    // 2. El valor de RSP
    // 4. Algo del timertick
}

void initializeRegisters(uint64_t rsp){
    StackedRegisters stackedRegisters = (StackedRegisters){0};
    stackedRegisters.rflags = 0x202;
    stackedRegisters.cs = 0x8;
    stackedRegisters.rip = default_rip;
    stackedRegisters.rsp = rsp;

    // TODO: revisar esto, porque tengo un mal presentimiento de esto
    memcpy( (void*) rsp, &stackedRegisters, sizeof(struct StackedRegisters));
}


void create_process(char *name, int argc, char **argv) {
    uint64_t new_pid = request_pid();
    uint64_t rsp = stacks[new_pid] + STACK_SIZE - ALIGN;      // TODO: no sé si va este - 64
    
    // arriba del RSP, hay que poner los valores de RAX, RBX, etc.
    initializeRegisters(rsp);

    blocks[new_pid].pid = new_pid;
    blocks[new_pid].stack_pointer = rsp;
    blocks[new_pid].process_state = READY;
    blocks[new_pid].parent_pid = running_pid;
    blocks[new_pid].priority = 1;
    blocks[new_pid].p_name = name;
    blocks[new_pid].argc = argc;
    blocks[new_pid].argv = argv;

    add(&round_robin, new_pid);

    // forzar timer-tick
}

void create_init_process(){
    blocks[0].pid = 0;
    blocks[0].stack_pointer = 0;    // Se va a actualizar. El valor no importa
    blocks[0].process_state = RUNNING;
    blocks[0].parent_pid = -1;       // TODO: designarle un valor. Por ahora, no importa mucho
    blocks[0].priority = 1;
    blocks[0].p_name = 0;
    running_pid = 0;
    printf("Voy a inicializar\n");
    initializer();

    add(&round_robin, 0);

    // OBS: el proceso INIT va a usar otro stack
}

void create_process_beta(uint64_t rip) {
    uint64_t new_pid = request_pid();
    uint64_t rsp = stacks[new_pid] + STACK_SIZE - ALIGN;      // TODO: no sé si va este - 64
    
    // arriba del RSP, hay que poner los valores de RAX, RBX, etc.
    initializeRegisters(rsp);

    blocks[new_pid].pid = new_pid;
    blocks[new_pid].stack_pointer = rsp;
    blocks[new_pid].process_state = READY;
    blocks[new_pid].parent_pid = running_pid;
    blocks[new_pid].priority = 1;

    add(&round_robin, new_pid);

    // forzar timer-tick
    
}

// ------- End --------

/// --------- Syscalls --------



int64_t getpid() {
    return running_pid;
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
