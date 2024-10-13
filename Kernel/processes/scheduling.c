#include "scheduling.h"
#include "round_robin.h"
#include "lib.h"
#include "IO.h"
#include "process.h"
#include "interrupts.h"

Stack stacks[MAX_PROCESS_BLOCKS] = {0}; 

uint64_t running_pid = 0;
CircularList round_robin = {0};
int remaining_quantum = 0;

typedef struct ProcessBlock
{
    uint64_t stack_pointer;
    State process_state;
    uint64_t parent_pid;
    uint64_t priority;
    char *p_name;
    int argc;
    char **argv;
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
            printf("[scheduler] RAN OUT OF PID!!\n");
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

void exit(uint64_t return_value)
{
    kill_process(get_pid());
    force_timer_tick();
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
    if (pid > MAX_PROCESS_BLOCKS || pid < 1)
        return -1;
    delete_value(&round_robin, pid);
    reasign_children(pid);

    blocks[pid].argc = 0;
    blocks[pid].argv = NULL;
    blocks[pid].p_name = NULL;
    blocks[pid].parent_pid = 0;
    blocks[pid].priority = 0;
    blocks[pid].process_state = UNAVAILABLE;
    blocks[pid].stack_pointer = 0;
    free_pid(pid);
    return pid;
}

int get_process_status(uint64_t pid)
{
    return blocks[pid].process_state;
}


uint64_t schedule(uint64_t running_stack_pointer)
{
    if(remaining_quantum > 0) {
        remaining_quantum--;
        return running_stack_pointer;
    }
    remaining_quantum = 2;
    blocks[running_pid].stack_pointer = running_stack_pointer;
    blocks[running_pid].process_state = READY;

    uint64_t next_pid = 0;
    do {
        next_pid = next(&round_robin);
    } while(blocks[next_pid].process_state == BLOCKED);

    running_pid = next_pid;

    blocks[next_pid].process_state = RUNNING;
    return blocks[next_pid].stack_pointer;
}


// ------- Principio de un proceso --------


uint64_t default_rip = 0;

void initializer()
{
    default_rip = _get_starting_point();
    // a partir de acá empieza un proceso
    printf("Empezo el proceso nro ");
    k_print_int_dec(running_pid);
    putChar('\n');

    // TODO: RBP = 0, arreglarlo
    // Ojo: esto implica que NO SE PUEDEN crear variables en este stackframe  

    if (blocks[running_pid].p_name == 0)
        return; // para el proceso init
    
    process_initializer(
        blocks[running_pid].p_name,
        blocks[running_pid].argc,
        blocks[running_pid].argv);
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
    memset(stacks[new_pid], 0, sizeof(Stack));
    uint64_t rsp = stacks[new_pid] + STACK_SIZE; 
    for(int i = 0; i < argc; i++) {
        rsp -= sizeof(char *);
        char **stack_argument = (char **)rsp;
        *stack_argument = argv[argc - i - 1];
    }
    blocks[new_pid].argv = rsp;
    rsp -= sizeof(StackedRegisters);

    initializeRegisters(rsp);

    blocks[new_pid].stack_pointer = rsp;
    blocks[new_pid].process_state = READY;
    blocks[new_pid].parent_pid = running_pid;
    blocks[new_pid].priority = 1;
    blocks[new_pid].p_name = name;
    blocks[new_pid].argc = argc;

    add(&round_robin, new_pid);
    return new_pid;
}

void create_init_process()
{
    int pid = request_pid(); 
    // Should be 0
    blocks[0].stack_pointer = 0;   // Se va a actualizar. El valor no importa
    blocks[0].process_state = RUNNING;
    blocks[0].parent_pid = get_pid(); 
    blocks[0].priority = 1;
    blocks[0].p_name = 0;
    running_pid = 0;
    initializer();

    add(&round_robin, 0);

    // OBS: el proceso INIT va a usar otro stack
}


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

void get_all_processes()
{
    static char *PROCESS_STATE_STRING[] = {"U", "RUNNING", "READY", "BLOCKED"};
    printf("pid process_name priority rsp state\n");
    for (int i = 0; i < MAX_PROCESS_BLOCKS; i++)
    {
        if (blocks[i].process_state != UNAVAILABLE)
        {
            k_print_int_dec(i);
            putChar(' ');
            printf(blocks[i].p_name);
            putChar(' ');
            k_print_int_dec(blocks[i].priority);
            putChar(' ');

            k_print_int_dec(blocks[i].stack_pointer);
            putChar(' ');
            printf(PROCESS_STATE_STRING[blocks[i].process_state]);
            putChar('\n');
        }
    }
}

void yield()
{
    force_timer_tick();
}

void change_priority(uint64_t pid, int delta)
{
    if (delta > 0)
    {
        while(delta-- > 0 && blocks[pid].priority < MAX_PRIORITY) 
        {
            blocks[pid].priority++;
            add(&round_robin, pid);
        }
    }
    else if(delta < 0)
    {
        while(delta++ < 0 && blocks[pid].priority > 1) {
            blocks[pid].priority--;
            delete_value_ocurrence(&round_robin, pid);
        }
    }
}

int block(int pid)
{
    if (pid > 64 || pid < 1 || blocks[pid].process_state == UNAVAILABLE)
        return -1;
    
    blocks[pid].process_state = BLOCKED;
    if(get_pid() == pid) {
        yield();
    }
    return pid;
}

int unlock(int pid)
{
    if (pid > 64 || pid < 1)
        return -1;

    blocks[pid].process_state = READY;
    return pid;
}

uint64_t wait_pid(uint64_t pid, int *status, int options)
{
    if ((pid > 0 || pid < -1) && blocks[pid].parent_pid == get_pid())
    {
        return blocks[pid < 0 ? pid * -1 : pid].process_state == UNAVAILABLE ? (pid < 0 ? pid * -1 : pid) : -1;
    }
    else if (pid == 0 || pid == -1)
    {
        for (int i = 0; i < MAX_PROCESS_BLOCKS; i++)
        {
            if (blocks[i].parent_pid == get_pid() && blocks[i].process_state == UNAVAILABLE)
            {
                return i;
            }
        }
    }
}
