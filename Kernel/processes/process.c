// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "process.h"
#include "lib.h"
#include "IO.h"
#include "interrupts.h"
#include "pipes.h"
#include "scheduler.h"
#include "sleep_manager.h"
#include "time.h"
#include "sound.h"
#include "tickets.h"
#include "registers.h"


typedef struct ProcessBlock
{
    uint64_t stack_pointer;
    uint64_t rbp;
    State process_state;
    pid_t parent_pid;
    Priority priority;
    Program program;
    int argc;
    char **argv;
    fd_t file_descriptors[MAX_FILE_DESCRIPTORS];
    pid_t pid_to_wait;
    uint8_t is_sleeping;
    StdinOption stdin_options;
} ProcessBlock;

Stack stacks[MAX_PROCESS_BLOCKS] = {0}; 
ProcessBlock blocks[MAX_PROCESS_BLOCKS] = {0};
pid_t running_pid = 0;
pid_t foreground_pid = 0;
Tickets tickets_pid = {0};
int64_t last_exit_code = 0;

void exit(int64_t return_value)
{
    kill_process(get_pid(), 0);
    last_exit_code = return_value;
    yield();
}

int64_t get_last_exit_code() {
    return last_exit_code;
}

static void reasign_children(pid_t pid, int recursive)
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

pid_t kill_process(pid_t pid, int recursive)
{
    if (pid >= MAX_PROCESS_BLOCKS || pid < 1)
        return -1;
    
    if (pid == 1) {
        setup_kernel_restart();
    }

    if (blocks[pid].is_sleeping)
    {
        wake_forced(pid);
    }

    if (get_player() == pid)
    {
        nosound();
    }
    
    reasign_children(pid, recursive);
    int ppid = blocks[pid].parent_pid;
    if (pid == foreground_pid) 
    {
        set_foreground(1);
    }
    if(blocks[ppid].pid_to_wait == pid) 
    {
        unblock(ppid);
    }
    blocks[pid].argc = 0;
    blocks[pid].argv = NULL;
    blocks[pid].program = NULL;
    blocks[pid].parent_pid = 0;
    Priority priority = blocks[pid].priority;
    blocks[pid].priority = 0;
    blocks[pid].process_state = UNAVAILABLE;
    blocks[pid].stack_pointer = 0;

    for(int i = 0; i < MAX_FILE_DESCRIPTORS; i++) 
    {
        fd_t pipe_id = blocks[pid].file_descriptors[i];
        if(pipe_id >= STD_FILE_DESCRIPTORS) 
        {
            close_pipe_end(pipe_id);
        }
        blocks[pid].file_descriptors[i] = DEVNULL;
    }
    last_exit_code = -1;
    free_ticket(&tickets_pid, pid);
    scheduler_remove(priority, pid);

    return pid;
}

int get_process_status(pid_t pid)
{
    return blocks[pid].process_state;
}

uint64_t schedule(uint64_t running_stack_pointer)
{
    if(scheduler_consume_quantum() > 0) {
        return running_stack_pointer;
    }

    wake_available();

    blocks[running_pid].stack_pointer = running_stack_pointer;
    blocks[running_pid].rbp = ((StackedRegisters *)running_stack_pointer)->rbp;
    if(blocks[running_pid].process_state == RUNNING) {
        blocks[running_pid].process_state = READY;
    }

    running_pid = scheduler_next_pid();
    blocks[running_pid].process_state = RUNNING;

    return blocks[running_pid].stack_pointer;
}

void set_foreground(pid_t pid) {
    foreground_pid = pid;
    blocks[pid].file_descriptors[STDIN] = STDIN;
    change_priority(pid, PRIORITY_HIGH);
    unblock(pid);
}

pid_t get_foreground() {
    return foreground_pid;
}

static void process_entry_point()
{
    exit(
        blocks[running_pid].program(blocks[running_pid].argc, (const char **) blocks[running_pid].argv)
    );
}

static uint64_t initialize_stack(pid_t new_pid, int argc, char **argv)
{
    memset(stacks[new_pid], 0, sizeof(Stack));
    uint64_t rsp = (uint64_t)&stacks[new_pid][STACK_SIZE - sizeof(char *)]; 
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
    rsp = (rsp & -16) - sizeof(StackedRegisters);
    blocks[new_pid].argv = (char **)rsp_argv;
    StackedRegisters *stacked_registers = (StackedRegisters *)rsp;

    stacked_registers->rflags = 0x202;
    stacked_registers->cs = 0x8;
    stacked_registers->rip = (uint64_t)process_entry_point;
    stacked_registers->rsp = rsp;
    stacked_registers->rbp = rsp + sizeof(StackedRegisters);

    blocks[new_pid].rbp = stacked_registers->rbp;

    return rsp;
}

pid_t create_process(Program program, int argc, char **argv, fd_t fds[])
{
    pid_t new_pid = request_ticket(&tickets_pid);
    if (new_pid == INVALID_PID)
    {
        return INVALID_PID;
    }

    blocks[new_pid].stack_pointer = initialize_stack(new_pid, argc, argv);
    blocks[new_pid].process_state = READY;
    blocks[new_pid].parent_pid = running_pid;
    blocks[new_pid].priority = PRIORITY_MID;
    blocks[new_pid].program = program;
    blocks[new_pid].argc = argc;
    blocks[new_pid].pid_to_wait = 0;

    for(int i = 0; i < STD_FILE_DESCRIPTORS; i++) 
    {
        blocks[new_pid].file_descriptors[i] = fds[i];
        if (fds[i] >= STD_FILE_DESCRIPTORS)
        {
            assign_pipe_to_process(fds[i], new_pid);
        }
    }

    scheduler_insert(PRIORITY_MID, new_pid);
    return new_pid;
}

void create_init_process()
{
    static char *init_args[] = {"INIT"};
    memset(blocks, 0, sizeof(blocks));
    memset(stacks, 0, sizeof(stacks));
    initialize_tickets(&tickets_pid, stacks, sizeof(Stack), MAX_PROCESS_BLOCKS);

    pid_t pid = request_ticket(&tickets_pid); 

    running_pid = 0;
    last_exit_code = 0;
    blocks[0].stack_pointer = 0;   // Se va a actualizar. el proceso INIT va a usar otro stack
    blocks[0].process_state = RUNNING;
    blocks[0].parent_pid = get_pid(); 
    blocks[0].priority = PRIORITY_LOW;
    blocks[0].argc = sizeof(init_args) / sizeof(init_args[0]);
    blocks[0].argv = init_args;
    blocks[0].file_descriptors[STDIN] = DEVNULL;
    blocks[0].file_descriptors[STDOUT] = STDERR;
    blocks[0].file_descriptors[STDERR] = STDERR;

    scheduler_initialize();
    scheduler_insert(blocks[0].priority, pid);
}

void set_pending_action(PendingAction action) {
    pid_t pid = foreground_pid;
    int ppid = blocks[pid].parent_pid;
    switch (action)
    {
    case KILL_FOREGROUND:
        printf_color("^C Process terminated pid=%d\n", COLOR_YELLOW, pid);
        kill_process(pid, 1);
        break;
    case BLOCK_FOREGROUND:
        printf_color("^Z Process blocked and sent to background pid=%d\n", COLOR_YELLOW, pid);
        block(pid);
        blocks[pid].file_descriptors[STDIN] = DEVNULL;
        change_priority(pid, PRIORITY_MID);
        set_foreground(1);
        if(blocks[ppid].pid_to_wait == pid) {
            blocks[ppid].pid_to_wait = 0;
            unblock(ppid);
        }
        break;
    case FOREGROUND_TO_BACKGROUND:
        printf_color("^X Process sent to run in background pid=%d\n", COLOR_YELLOW, pid);
        blocks[pid].file_descriptors[STDIN] = DEVNULL;
        change_priority(pid, PRIORITY_MID);
        set_foreground(1);
        if(blocks[ppid].pid_to_wait == pid) {
            blocks[ppid].pid_to_wait = 0;
            unblock(ppid);
        }
        break;
    default:
        break;
    }
    yield();
}

pid_t get_pid()
{
    return running_pid;
}

void get_all_processes()
{
    static char *PROCESS_STATE_STRING[] = {"UNKNOWN", "RUNNING", "READY  ", "BLOCKED"};
    static uint64_t PROCESS_STATE_COLOR[] = {0x00999999, 0x0000FF00, 0x00CCDD00, 0x00FF0000};
    static char *PRIORITY_STRING[] = {"LOW ", "MID ", "HIGH", "NONE"};
    printf("pid : ppid : prio : stack_pointer_64 : base_pointer_64  : status  : process_name\n");
    for (int i = 0; i < MAX_PROCESS_BLOCKS; i++)
    {
        if (blocks[i].process_state != UNAVAILABLE)
        {
            printf("%3d : %4ld : ", i, blocks[i].parent_pid);
            int priority = blocks[i].priority;
            printf_color("%s", PROCESS_STATE_COLOR[priority+1], PRIORITY_STRING[priority]);
            printf(" : %16lx : %16lx : ", blocks[i].stack_pointer, blocks[i].rbp);
            uint64_t process_state = blocks[i].process_state;
            printf_color(PROCESS_STATE_STRING[process_state], PROCESS_STATE_COLOR[process_state]);
            printf(" : %s\n", blocks[i].argc > 0 ? blocks[i].argv[0] : "UNKNOWN PROCESS");
        }
    }
}

void change_priority(pid_t pid, int value){
    if(pid == 0 || value > PRIORITY_HIGH || blocks[pid].process_state == UNAVAILABLE) {
        return;
    }
    Priority previous_priority = blocks[pid].priority;
    if(previous_priority != value) {
        scheduler_remove(previous_priority, pid);
        if(blocks[pid].process_state != BLOCKED)
            scheduler_insert(value, pid);
        blocks[pid].priority = value;
    }
}

pid_t block(pid_t pid)
{
    if(block_no_yield(pid) == -1) 
        return -1;
    if(get_pid() == pid) {
        yield();
    }
    return pid;
}

void sys_set_fd(pid_t pid, fd_t fd_index, fd_t fd) {
    if(pid > 0) {
        blocks[pid].file_descriptors[fd_index] = fd;
    }
}

pid_t block_no_yield(pid_t pid) {
    if (pid >= MAX_PROCESS_BLOCKS || pid < 1 || blocks[pid].process_state == UNAVAILABLE)
        return -1;
    blocks[pid].process_state = BLOCKED;
    scheduler_remove(blocks[pid].priority, pid);
    return 0;
}

pid_t unblock(pid_t pid)
{
    if (pid >= MAX_PROCESS_BLOCKS || pid < 1 || blocks[pid].process_state != BLOCKED || blocks[pid].is_sleeping)
        return -1;
    blocks[pid].process_state = READY;
    scheduler_insert(blocks[pid].priority, pid);
    return pid;
}

pid_t wait_pid(pid_t pid)
{
    if(blocks[pid].process_state == UNAVAILABLE) return pid;
    int ppid = blocks[pid].parent_pid;
    blocks[ppid].pid_to_wait = pid;
    block(ppid);

    return pid;
}


pid_t get_fd(fd_t index){
    if (index >= MAX_FILE_DESCRIPTORS || index < 0) return DEVNULL;
    return blocks[get_pid()].file_descriptors[index];
}

void set_sleeping_state(uint8_t is_sleeping, pid_t pid)
{
    blocks[pid].is_sleeping = is_sleeping;
}

StdinOption get_stdin_options()
{
    return blocks[running_pid].stdin_options;
}

void set_stdin_options(StdinOption stdin_option)
{
    blocks[running_pid].stdin_options = stdin_option;
}
