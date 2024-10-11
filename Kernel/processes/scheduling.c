#include "scheduling.h"
#include "round_robin.h"
#include "lib.h"
#include "IO.h"
#include "process.h"
#include "interrupts.h"

Stack stacks[MAX_PROCESS_BLOCKS] = {0};

uint64_t running_pid = 0;
CircularList round_robin = {0};

typedef struct ProcessBlock
{
    uint64_t stack_pointer;
    State process_state;
    uint64_t pid; // <---- stacks[pid] es stack_base ; blocks[i] determina i=pid
    uint64_t parent_pid;
    uint64_t priority;
    char *p_name;
    int argc;
    char **argv;
    // char **argv;
} ProcessBlock;

uint64_t available_pids[MAX_PROCESS_BLOCKS] = {0};
uint64_t current_available_pid_index = 0;
uint64_t biggest_pid = 0;

uint64_t request_pid()
{
    if (current_available_pid_index >= biggest_pid)
    {
        if (biggest_pid >= MAX_PROCESS_BLOCKS)
        {
            printf("RAN OUT OF PID!!\n");    // Por alguna razón no anda
            return INVALID_PID;
        }
        biggest_pid++;
        return current_available_pid_index++;
    }
    return available_pids[current_available_pid_index++];
}

void free_pid(int pid)
{
    available_pids[--current_available_pid_index] = pid;
}

ProcessBlock blocks[MAX_PROCESS_BLOCKS] = {0};
// TODO: hacer que tire error cuando pid > 64

void exit(uint64_t return_value)
{
    return;
};

void reasign_children(uint64_t pid)
{
    for (int i = 0; i < MAX_PROCESS_BLOCKS; i++)
    {
        if (blocks[i].parent_pid == pid)
        {
            blocks[i].parent_pid = 0;
        }
    }
}

int kill_process(uint64_t pid)
{
    if (pid > 64 || pid < 1)
        return -1;
    delete_value(&round_robin, pid);
    reasign_children(pid);

    memset(stacks[pid], 0, sizeof(Stack));
    blocks[pid].argc = 0;
    blocks[pid].argv = NULL;
    blocks[pid].p_name = NULL;
    blocks[pid].parent_pid = 0;
    blocks[pid].pid = INVALID_PID;
    blocks[pid].priority = 0;
    blocks[pid].process_state = UNAVAILABLE;
    blocks[pid].stack_pointer = 0;
    free_pid(pid);
    return pid;
}

void store_context()
{
}

int get_process_status(uint64_t pid)
{
    return blocks[pid].process_state;
}

// --------- scheduler --------

uint64_t schedule(uint64_t running_stack_pointer)
{
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

#define ALIGN 40

uint64_t default_rip = 0;

void initializer()
{
    default_rip = _get_starting_point();
    // a partir de acá empieza un proceso
    printf("Empezo el proceso nro ");
    k_print_int_dec(running_pid);
    putChar('\n');

    // Ojo: acá NO SE PUEDEN asigar variables

    if (blocks[running_pid].p_name == 0)
        return; // para el proceso init
    exit(
        process_initializer(
            blocks[running_pid].p_name,
            blocks[running_pid].argc,
            blocks[running_pid].argv));

    // Cosas que pueden estar mal:
    // 1. El orden del struct de StackedRegisters
    // 2. El valor de RSP
    // 4. Algo del timertick
}

void initializeRegisters(uint64_t rsp)
{
    StackedRegisters stackedRegisters = (StackedRegisters){0};
    stackedRegisters.rflags = 0x202;
    stackedRegisters.cs = 0x8;
    stackedRegisters.rip = default_rip;
    stackedRegisters.rsp = rsp;

    // TODO: revisar esto, porque tengo un mal presentimiento de esto
    memcpy((void *)rsp, &stackedRegisters, sizeof(struct StackedRegisters));
}

int create_process(char *name, int argc, char **argv)
{
    uint64_t new_pid = request_pid();
    if (new_pid == INVALID_PID)
    {
        return -1;
    }
    uint64_t rsp = stacks[new_pid] + STACK_SIZE - ALIGN; // TODO: no sé si va este - 64

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
    return new_pid;
    // forzar timer-tick
}

void create_init_process()
{
    blocks[0].pid = request_pid(); // Should be 0
    blocks[0].stack_pointer = 0;   // Se va a actualizar. El valor no importa
    blocks[0].process_state = RUNNING;
    blocks[0].parent_pid = -1; // TODO: designarle un valor. Por ahora, no importa mucho
    blocks[0].priority = 1;
    blocks[0].p_name = 0;
    running_pid = 0;
    printf("Voy a inicializar\n");
    initializer();

    add(&round_robin, 0);

    // OBS: el proceso INIT va a usar otro stack
}

void create_process_beta(uint64_t rip)
{
    uint64_t new_pid = request_pid();
    uint64_t rsp = stacks[new_pid] + STACK_SIZE - ALIGN; // TODO: no sé si va este - 64

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

// ------- Utilities --------

int get_processes_count()
{
    int count = 0;
    for (int i = 0; i < MAX_PROCESS_BLOCKS; i++)
    {
        if (blocks[i].process_state != UNAVAILABLE)
        {
            count++;
        }
    }
    return count;
}

/// --------- Syscalls --------

int64_t get_pid()
{
    return running_pid;
}

void info_processes()
{
    // ready --> pcb
}

void get_all_processes()
{
    if (free_pid == 0)
    {
        printf("No hay procesos\n");
        return;
    }
    printf("pid process_name priority rsp \n");
    for (int i = 0; i < MAX_PROCESS_BLOCKS; i++)
    {
        if (blocks[i].process_state != UNAVAILABLE)
        {
            k_print_int_dec(blocks[i].pid);
            putChar(' ');
            printf(blocks[i].p_name);
            k_print_int_dec(blocks[i].priority);
            // print stack

            k_print_int_dec(blocks[i].stack_pointer);
            putChar('\n');
            // toReturn[i] = (ProcessSnapshot){
            //     .p_name = blocks[i].p_name,
            //     .pid = blocks[i].pid,
            //     .priority = blocks[i].priority,
            //     .stack = stacks[i],
            //     .base_pointer = blocks[i].stack_pointer,
            //     .foreground = 0
            //     /* ACA HAY QUE PONER CREO LOS FD DE LOS HIJOS? */
            // };
        }
    }
    // return toReturn;
}

void yield()
{
    next(&round_robin);
}

void change_priority(uint64_t pid, int value)
{ // no estoy seguro de que esté bien
    if (value == 0)
        return;
    if (value > 0 && value < 5)
    {
        delete_value_ocurrence(&round_robin, pid);
    }
    else
    {
        add(&round_robin, pid);
    }
}

int block(int pid)
{
    if (pid > 64 || pid < 1)
        return -1;
    // manda proceso de RUNNING a BLOCKED
    blocks[running_pid].process_state = BLOCKED;
    return pid;
}

int unlock(int pid)
{
    if (pid > 64 || pid < 1)
        return -1;
    // manda proceso de BLOCKED a READY con prioridad = 1
    blocks[pid].process_state = READY;
    return pid;
}

void resume()
{
    // ni idea lo que hace
}

uint64_t wait_pid(uint64_t pid, int *status, int options)
{
    if ((pid > 0 || pid < -1) && blocks[pid].parent_pid == get_pid())
    {
        return blocks[pid < 0 ? pid * -1 : pid].process_state == UNAVAILABLE ? (pid < 0 ? pid * -1 : pid) : -1;
    }
    else if (pid == 0)
    {
        for (int i = 0; i < MAX_PROCESS_BLOCKS; i++)
        {
            if (blocks[i].parent_pid == get_pid() && blocks[i].process_state == UNAVAILABLE)
            {
                return blocks[i].pid;
            }
        }
    }
    else if (pid == -1)
    {
        for (int i = 0; i < MAX_PROCESS_BLOCKS; i++)
        {
            if (blocks[i].parent_pid == get_pid() && blocks[i].process_state == UNAVAILABLE)
            {
                return blocks[i].pid;
            }
        }
    }
}

// --------- end syscalls --------
