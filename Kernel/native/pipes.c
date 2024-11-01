#include "pipes.h"
#include "lib.h"
#include "IO.h"
#include "bounded_queue2.h"
#include "semaphores.h"

// TODO: que cada pipe tenga un semáforo distinto?
#define MUTEX 30
#define MAX_PIPES 10

typedef struct
{
    queue2_t buffer;
    char available;
    int64_t blocked_pid;    // Desventaja: solo puede haber un elemento bloqueado.
} pipe_t;


pipe_t pipes[MAX_PIPES] = { 0 };

uint64_t available_pipes[MAX_PIPES] = {0};
uint64_t current_available_pipe_index = 0;
uint64_t biggest_pipe_id = 0;

int64_t pipe_to_fd(int64_t pipe)
{
    return pipe >= 0 ? pipe + 3 : pipe;
}

int64_t fd_to_pipe(int64_t fd)
{
    return fd >= 3 ? fd - 3 : fd;
}

int64_t request_pipe()
{
    if (current_available_pipe_index >= biggest_pipe_id)
    {
        if (biggest_pipe_id >= MAX_PIPES)
        {
            printf_error("[pipes] RAN OUT OF PIPES!!\n");
            return -1;
        }
        biggest_pipe_id++;
        return pipe_to_fd(current_available_pipe_index++);
    }
    return pipe_to_fd(available_pipes[current_available_pipe_index++]);
}

char pipe_is_valid(int pipe)
{
    return pipe >=0 && pipe < MAX_PIPES && pipes[pipe].available;
}

int8_t create_pipe(int fd)
{
    int64_t pipe = fd_to_pipe(fd);
    if (pipe > MAX_PIPES || pipe < 0 || pipes[pipe].available == 1) return 0;

    sem_open(MUTEX, 1);
    sem_wait(MUTEX);

    pipes[pipe].available = 1;
    init_queue2(&pipes[pipe].buffer);
    pipes[pipe].blocked_pid = -1;

    sem_post(MUTEX);
    
    return 1;
}

int8_t close_pipe(int fd) {
    int64_t pipe = fd_to_pipe(fd);
    if (!pipe_is_valid(pipe)) return 0;

    pipes[pipe].available = 0;
    available_pipes[--current_available_pipe_index] = pipe;
    return 1;
}

int64_t read_pipe(int fd, char* buf, int count)
{
    int64_t pipe = fd_to_pipe(fd);

    if (!pipe_is_valid(pipe)) return 0;
    
    uint64_t to_return;
    do
    {
        sem_wait(MUTEX);
        to_return = dequeue_to_buffer2(&pipes[pipe].buffer, buf, count);
        if(!to_return)
        {
            int64_t current_pid = get_pid();
            pipes[pipe].blocked_pid = current_pid;
            block_no_yield(current_pid);
            sem_post(MUTEX);
            yield();
        }
    } while(!to_return);
    sem_post(MUTEX);

    if (pipes[pipe].blocked_pid != -1)
    {
        unblock(pipes[pipe].blocked_pid);
        pipes[pipe].blocked_pid = -1;
    }
    return to_return;
}

int64_t write_pipe(int fd, const char* buf, int count)
{
    int64_t pipe = fd_to_pipe(fd);

    if (!pipe_is_valid(pipe)) return 0;

    uint64_t written;
    do
    {
        sem_wait(MUTEX);
        written += enqueue_string2(&pipes[pipe].buffer, buf, count);
        if(written < count)
        {
            int64_t current_pid = get_pid();
            pipes[pipe].blocked_pid = current_pid;
            block_no_yield(current_pid);
            sem_post(MUTEX);
            yield();
        }
    } while(written < count);
    sem_post(MUTEX);

    if (pipes[pipe].blocked_pid != -1)
    {
        unblock(pipes[pipe].blocked_pid);
        pipes[pipe].blocked_pid = -1;
    }
    return count;
}