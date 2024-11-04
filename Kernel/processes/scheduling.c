// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "scheduling.h"
// #include "round_robin.h"
#include "lib.h"
#include "IO.h"
#include "interrupts.h"
#include "pipes.h"
#include "semaphores.h"
#include "scheduler.h"

Stack stacks[MAX_PROCESS_BLOCKS] = {0}; 

uint64_t running_pid = 0;
// CircularList round_robin = {0};
int remaining_quantum = 0;
PendingAction pending_action = NONE;

StackedRegisters saved_regs = {0};
char regs_captured = 0;

typedef struct ProcessBlock
{
    uint64_t stack_pointer;
    State process_state;
    uint64_t parent_pid;
    uint64_t priority;
    Program program;
    int argc;
    char **argv;
    int file_descriptors[MAX_FILE_DESCRIPTORS];
    int pid_to_wait;
    StackedRegisters regs;
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
            printf_error("[scheduler] RAN OUT OF PID!!\n");
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
    kill_process(get_pid(), 0);
    yield();
}

static void reasign_children(uint64_t pid, int recursive)
{
    for (int i = 0; i < MAX_PROCESS_BLOCKS; i++)
    {
        if (blocks[i].parent_pid == pid)
        {
            if(recursive)
                kill_process(i, recursive);
            else 
                blocks[i].parent_pid = 0;
        }
    }
}

int kill_process(uint64_t pid, int recursive)
{
    if (pid >= MAX_PROCESS_BLOCKS || pid < 1)
        return -1;
    
    if(pid == 1) {
        setup_kernel_restart();
    }
    
    reasign_children(pid, recursive);
    int ppid = blocks[pid].parent_pid;
    if(blocks[ppid].pid_to_wait == pid) {
        unblock(ppid);
    }
    blocks[pid].argc = 0;
    blocks[pid].argv = NULL;
    blocks[pid].program = NULL;
    blocks[pid].parent_pid = 0;
    int priority = blocks[pid].priority;
    blocks[pid].priority = 0;
    blocks[pid].process_state = UNAVAILABLE;
    blocks[pid].stack_pointer = 0;

    for(int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        int pipe_id = blocks[pid].file_descriptors[i];
        if(pipe_id >= STD_FILE_DESCRIPTORS) {
            close_pipe(pipe_id);
        }
        blocks[pid].file_descriptors[i] = DEV_NULL;
    }
    free_pid(pid);
    scheduler_remove(priority, pid);
    // delete_value(&round_robin, pid);

    return pid;
}

int get_process_status(uint64_t pid)
{
    return blocks[pid].process_state;
}

void save_regs() {
    saved_regs = blocks[running_pid].regs;
    regs_captured = 1;
}

void print_regs(StackedRegisters regs){
    printf("\nRAX : 0x"); k_print_integer(regs.rax, 16, 16);
    printf("\nRBX : 0x"); k_print_integer(regs.rbx, 16, 16);
    printf("\nRCX : 0x"); k_print_integer(regs.rcx, 16, 16);
    printf("\nRDX : 0x"); k_print_integer(regs.rdx, 16, 16);
    printf("\nRDI : 0x"); k_print_integer(regs.rdi, 16, 16);
    printf("\nRSI : 0x"); k_print_integer(regs.rsi, 16, 16);
    printf("\nRBP : 0x"); k_print_integer(regs.rbp, 16, 16);
    printf("\nRSP : 0x"); k_print_integer(regs.rsp, 16, 16);
    printf("\nR08 : 0x"); k_print_integer(regs.r8, 16, 16);
    printf("\nR09 : 0x"); k_print_integer(regs.r9, 16, 16);
    printf("\nR10 : 0x"); k_print_integer(regs.r10, 16, 16);
    printf("\nR11 : 0x"); k_print_integer(regs.r11, 16, 16);
    printf("\nR12 : 0x"); k_print_integer(regs.r12, 16, 16);
    printf("\nR13 : 0x"); k_print_integer(regs.r13, 16, 16);
    printf("\nR14 : 0x"); k_print_integer(regs.r14, 16, 16);
    printf("\nR15 : 0x"); k_print_integer(regs.r15, 16, 16);
    printf("\nRIP : 0x"); k_print_integer(regs.rip, 16, 16);
    printf("\nRFLAGS : 0x"); k_print_integer(regs.rflags, 16, 16);
    printf("\nCS : 0x"); k_print_integer(regs.cs, 16, 16);
    printf("\nSS : 0x"); k_print_integer(regs.ss, 16, 16);
    printf("\n\n");
}

void print_saved_regs()
{
    if (!regs_captured) {
        printf("No regs saved. Press left alt to save regs");
        print_regs(blocks[running_pid].regs);
    }
    else {
        print_regs(saved_regs);
    }
}

uint64_t schedule(uint64_t running_stack_pointer)
{
    if(remaining_quantum > 0) {
        remaining_quantum--;
        return running_stack_pointer;
    }
    remaining_quantum = scheduler_get_quantum(blocks[running_pid].priority);
    blocks[running_pid].stack_pointer = running_stack_pointer;

    if(blocks[running_pid].process_state == RUNNING) {
        blocks[running_pid].process_state = READY;
    }

    uint64_t next_pid = scheduler_next_pid();
    // do {
    //     next_pid = next(&round_robin);
    // } while(blocks[next_pid].process_state == BLOCKED);
    if(pending_action && blocks[next_pid].file_descriptors[STDIN] == STDIN && next_pid > 1 && blocks[next_pid].priority == PRIORITY_HIGH) {
        int ppid = blocks[next_pid].parent_pid;
        switch (pending_action)
        {
        case KILL_FOREGROUND:
            printf_color("^C Process terminated pid=%d\n", COLOR_YELLOW, next_pid);
            kill_process(next_pid, 1);
            break;
        case BLOCK_FOREGROUND:
            printf_color("^Z Process blocked and sent to background pid=%d\n", COLOR_YELLOW, next_pid);
            block(next_pid);
            blocks[next_pid].file_descriptors[STDIN] = DEV_NULL;
            change_priority(next_pid, PRIORITY_MID);
            if(blocks[ppid].pid_to_wait == next_pid) {
                blocks[ppid].pid_to_wait = 0;
                unblock(ppid);
            }
            break;
        case FOREGROUND_TO_BACKGROUND:
            printf_color("^X Process sent to run in background pid=%d\n", COLOR_YELLOW, next_pid);
            blocks[next_pid].file_descriptors[STDIN] = DEV_NULL;
            change_priority(next_pid, PRIORITY_MID);
            if(blocks[ppid].pid_to_wait == next_pid) {
                blocks[ppid].pid_to_wait = 0;
                unblock(ppid);
            }
            break;
        default:

            break;
        }
        pending_action = NONE;
        // do {
        //     next_pid = next(&round_robin);
        // } while(blocks[next_pid].process_state == BLOCKED);
        next_pid = scheduler_next_pid();
    }
    // printf("[%d]", next_pid);

    blocks[running_pid].regs = *(StackedRegisters *) running_stack_pointer;
    running_pid = next_pid;

    blocks[next_pid].process_state = RUNNING;
    return blocks[next_pid].stack_pointer;
}

void initializer()
{
    exit(
        blocks[running_pid].program(blocks[running_pid].argc, (const char **) blocks[running_pid].argv)
    );
}

void set_current_quantum(uint64_t q) {
    remaining_quantum = q;
}

void initializeRegisters(uint64_t new_pid, uint64_t rsp)
{
    StackedRegisters stackedRegisters = (StackedRegisters){0};
    stackedRegisters.rflags = 0x202;
    stackedRegisters.cs = 0x8;
    stackedRegisters.rip = (uint64_t)initializer;
    stackedRegisters.rsp = rsp;

    stackedRegisters.rbp = rsp + sizeof(stackedRegisters);

    blocks[new_pid].regs = stackedRegisters;
    memcpy((void *)rsp, &stackedRegisters, sizeof(struct StackedRegisters));
}

int create_process(Program program, int argc, char **argv, int fds[])
{
    uint64_t new_pid = request_pid();
    if (new_pid == INVALID_PID)
    {
        return -1;
    }
    memset(stacks[new_pid], 0, sizeof(Stack));
    uint64_t rsp = (uint64_t) stacks[new_pid] + STACK_SIZE; 
    uint64_t rsp_argv = rsp;
    rsp -= argc * sizeof(char **);
    for(int i = 0; i < argc; i++) {
        rsp_argv -= sizeof(char *);
        int j = 0;
        char *argument = argv[argc - i - 1];
        while(argument[j++]);
        while(j >= 0) {
            rsp--;
            char *current_char = (char *)rsp;
            *current_char = argument[j--];
        }

        char **stack_argument = (char **)rsp_argv;
        *stack_argument = (char *) rsp;
    }
    rsp &= -16;
    blocks[new_pid].argv = (char **)rsp_argv;
    rsp -= sizeof(StackedRegisters);

    initializeRegisters(new_pid, rsp);

    blocks[new_pid].stack_pointer = rsp;
    blocks[new_pid].process_state = READY;
    blocks[new_pid].parent_pid = running_pid;
    blocks[new_pid].priority = PRIORITY_MID;
    blocks[new_pid].program = program;
    blocks[new_pid].argc = argc;
    blocks[new_pid].pid_to_wait = 0;
    for(int i = 0; i < STD_FILE_DESCRIPTORS; i++) {
        blocks[new_pid].file_descriptors[i] = fds[i];
    }

    scheduler_insert(PRIORITY_MID, new_pid);
    // add(&round_robin, new_pid);
    return new_pid;
}

void create_init_process()
{
    static char *init_args[] = {"INIT"};
    running_pid = 0;
    int pid = request_pid(); 
    if(pid != 0) {
        // round_robin.current_index = 0;
        // round_robin.size = 0;
        memset(blocks, 0, sizeof(ProcessBlock) * MAX_PROCESS_BLOCKS);
        memset(available_pids, 0, sizeof(available_pids));
        biggest_pid = 0;
        current_available_pid_index = 0;
        pid = request_pid();
    }
    blocks[0].stack_pointer = 0;   // Se va a actualizar. el proceso INIT va a usar otro stack
    blocks[0].process_state = RUNNING;
    blocks[0].parent_pid = get_pid(); 
    blocks[0].priority = PRIORITY_LOW;
    blocks[0].argc = sizeof(init_args) / sizeof(init_args[0]);
    blocks[0].argv = init_args;
    blocks[0].file_descriptors[STDIN] = DEV_NULL;
    blocks[0].file_descriptors[STDOUT] = STDERR;
    blocks[0].file_descriptors[STDERR] = STDERR;

    scheduler_initialize();
    scheduler_insert(blocks[0].priority, pid);
    // add(&round_robin, 0);
}

void set_pending_action(PendingAction action) {
    pending_action = action;
}

int get_processes_count()
{
    return current_available_pid_index;
}

/// --------- Syscalls --------

int64_t get_pid()
{
    return running_pid;
}

void get_all_processes()
{
    static char *PROCESS_STATE_STRING[] = {"UNKNOWN", "RUNNING", "READY  ", "BLOCKED"};
    static uint64_t PROCESS_STATE_COLOR[] = {0x00999999, 0x0000FF00, 0x00CCDD00, 0x00FF0000};
    static char *PRIORITY_STRING[] = {"NONE", "LOW ", "MID ", "HIGH"};
    printf("pid : ppid : prio : stack_pointer_64 : base_pointer_64  : status  : process_name\n");
    for (int i = 0; i < MAX_PROCESS_BLOCKS; i++)
    {
        if (blocks[i].process_state != UNAVAILABLE)
        {
            printf("%3d : %4d : ", i, blocks[i].parent_pid);
            int priority = blocks[i].priority;
            printf_color("%s", PROCESS_STATE_COLOR[priority], PRIORITY_STRING[priority]);
            printf(" : %16x : %16x : ", blocks[i].stack_pointer, blocks[i].regs.rbp);
            uint64_t process_state = blocks[i].process_state;
            printf_color(PROCESS_STATE_STRING[process_state], PROCESS_STATE_COLOR[process_state]);
            printf(" : %s\n", blocks[i].argc > 0 ? blocks[i].argv[0] : "UNKNOWN PROCESS");
        }
    }
}

void yield()
{
    remaining_quantum = 0;
    force_timer_tick();
}


void change_priority(uint64_t pid, int value){
    // value = (value < MAX_PRIORITY) ? value : MAX_PRIORITY;
    // value = (value > 0) ? value : 1;
    if(pid == 0 || value <= PRIORITY_NONE || value > PRIORITY_HIGH || blocks[pid].process_state == UNAVAILABLE) {
        return;
    }
    Priority previous_priority = blocks[pid].priority;
    if(previous_priority != value) {
        scheduler_remove(previous_priority, pid);
        if(blocks[pid].process_state != BLOCKED)
            scheduler_insert(value, pid);
        blocks[pid].priority = value;
    }
    // int delta = value - blocks[pid].priority;

    // if (delta > 0)
    // {
    //     while(delta-- > 0) 
    //     {
    //         blocks[pid].priority++;
    //         add(&round_robin, pid);
    //     }
    // }
    // else if(delta < 0)
    // {
    //     while(delta++ < 0) 
    //     {
    //         blocks[pid].priority--;
    //         delete_value_ocurrence(&round_robin, pid);
    //     }
    // }
}

int block(int pid)
{
    if(block_no_yield(pid) == -1) 
        return -1;
    if(get_pid() == pid) {
        yield();
    }
    return pid;
}

void sys_set_fd(int pid, int fd_index, int value) {
    if(pid > 0) {
        blocks[pid].file_descriptors[fd_index] = value;
        if(fd_index == STDIN && value == STDIN) {
            force_enqueue(1, pid);
        }
    }
}

int block_no_yield(int pid) {
    if (pid >= MAX_PROCESS_BLOCKS || pid < 1 || blocks[pid].process_state == UNAVAILABLE)
        return -1;
    blocks[pid].process_state = BLOCKED;
    scheduler_remove(blocks[pid].priority, pid);
    return 0;
}

int unblock(int pid)
{
    if (pid >= MAX_PROCESS_BLOCKS || pid < 1)
        return -1;
    blocks[pid].process_state = READY;
    scheduler_insert(blocks[pid].priority, pid);
    return pid;
}

uint64_t wait_pid(uint64_t pid)
{
    if(blocks[pid].process_state == UNAVAILABLE) return pid;
    int ppid = blocks[pid].parent_pid;
    blocks[ppid].pid_to_wait = pid;
    block(ppid);

    return pid;
}


int64_t get_fd(int index){
    if (index >= MAX_FILE_DESCRIPTORS || index < 0) return DEV_NULL;
    return blocks[get_pid()].file_descriptors[index];
}