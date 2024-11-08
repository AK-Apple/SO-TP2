#include "pipes.h"
#include "lib.h"
#include "IO.h"
#include "bounded_queue2.h"
#include "semaphores.h"
#include "scheduler.h"

// TODO: que cada pipe tenga un semáforo distinto?
#define MUTEX 30
#define MAX_PIPES MAX_PROCESS_BLOCKS

typedef struct
{
    queue2_t buffer;
    char available;
    int64_t blocked_pid;    // Desventaja: solo puede haber un elemento bloqueado.
    int64_t writer_pid;
    int64_t reader_pid;
} pipe_t;




pipe_t pipes[MAX_PIPES] = { 0 };

uint64_t available_pipes[MAX_PIPES] = {0};
uint64_t current_available_pipe_index = 0;
uint64_t biggest_pipe_id = 0;

// ---------- Funciones de mapping de File Descriptors a Pipes y viceversa -------


static void pipe_to_fd(int64_t pipe, fd_t* fd_buffer)
{
    fd_buffer[0] = 2 * pipe + 3;       // buffer[0] = fd impar = read end
    fd_buffer[1] = 2 * pipe + 4;       // buffer[1] = fd par   = write end
}
// 0 <-> 3, 4
// 1 <-> 5, 6

static int64_t fd_to_pipe(fd_t fd)
{
    if (fd < 3){
        printf_error("File Descriptor [%d] will never point to a pipe. Must be >= 3\n", fd);
        return -1;
    }
    return (fd - 3) / 2;
}

static int8_t is_read_end(fd_t fd)
{
    return fd % 2 == 1; 
}

// -------------------- End --------------------

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
        return current_available_pipe_index++;
    }
    return available_pipes[current_available_pipe_index++];
}

char pipe_is_valid(int pipe)
{
    return pipe >=0 && pipe < MAX_PIPES && pipes[pipe].available;
}


int8_t create_pipe(fd_t* fd_buffer)
{
    int64_t pipe = request_pipe();
    if (pipe >= MAX_PIPES || pipe < 0 || pipes[pipe].available == 1) 
    {
        printf_error("Pipe couldnt be created\n");
        return 0;
    }

    sem_open(MUTEX, 1);
    sem_wait(MUTEX);

    pipes[pipe].available = 1;
    init_queue2(&pipes[pipe].buffer);
    pipes[pipe].blocked_pid = -1;
    pipes[pipe].reader_pid = -1;
    pipes[pipe].writer_pid = -1;

    sem_post(MUTEX);
    pipe_to_fd(pipe, fd_buffer);
    return 1;
}

// Requires a write_end of pipe = fd impar
int64_t read_pipe(fd_t fd, char* buf, int count)
{
    int64_t pipe = fd_to_pipe(fd);

    if (!pipe_is_valid(pipe) && is_empty2(&pipes[pipe].buffer)) {
        buf[0] = EOF;
        buf[1] = '\0';
        return 0;
    }

    if (is_read_end(fd))
    {
        printf_error("Cant read from the read-end part of a pipe of file descriptor %d. Pipe ID: [%d]\n", fd, pipe);
        return 0;
    }

    if (pipes[pipe].reader_pid != get_pid())
    {
        printf_error("Cant read from this pid [%d]. Pipe ID: %d, File Descriptor: %d\n", get_pid(), pipe, fd);
        return 0;
    }
    
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


// Requires a read_end of pipe = fd impar
int64_t write_pipe(fd_t fd, const char* buf, int count)
{
    int64_t pipe = fd_to_pipe(fd);

    if (!pipe_is_valid(pipe)) {
        printf_error("[kernel] cant write to a closed pipe [%d]%d \n", fd, pipe);
        return 0;
    }

    if (!is_read_end(fd))
    {
        printf_error("Cant write to the write-end part of a pipe of file descriptor %d. Pipe ID: [%d]\n", fd, pipe);
        return 0;
    }

    if (pipes[pipe].writer_pid != get_pid())
    {
        printf_error("Cant write from this pid [%d]. Should write from pid [%d] instead. Pipe ID: %d, File Descriptor: %d '%s'\n", get_pid(), pipes[pipe].reader_pid, pipe, fd, buf[0] == -1 ?"EOF":"N");
        return 0;
    }
    

    uint64_t written = 0;
    do
    {
        // TODO: opción para que no se bloquee si se llena el buffer y ya había un proceso bloqueado
        sem_wait(MUTEX);
        written += enqueue_string2(&pipes[pipe].buffer, buf, count);
        if(written < count)
        {
            if (pipes[pipe].blocked_pid != -1)
            {
                unblock(pipes[pipe].blocked_pid);
                pipes[pipe].blocked_pid = -1;
            }
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

void assign_pipe_to_process(fd_t fd, int pid)
{
    int64_t pipe = fd_to_pipe(fd);

    if (!pipe_is_valid(pipe)) {
        printf_error("[kernel] Wrong file descriptor [%d]. Pipe ID: %d \n", fd, pipe);
        return;
    }

    if (is_read_end(fd))
    {
        if (pipes[pipe].writer_pid == -1) 
        {
            pipes[pipe].writer_pid = pid;
        }
        else
        {
            printf_error("File descriptor [%d] is already occupied\n", fd);
        }
    }
    else
    {
        if (pipes[pipe].reader_pid == -1) 
        {
            pipes[pipe].reader_pid = pid;
        }
        else
        {
            printf_error("File descriptor [%d] is already occupied\n", fd);
        }
    }
}

void close_pipe_end(fd_t fd)
{
    int64_t pipe = fd_to_pipe(fd);

    if (!pipe_is_valid(pipe)) {
        return;
    }

    if (is_read_end(fd))
    {
        pipes[pipe].writer_pid = -1;
        char eof[] = {EOF, 0};
        enqueue_string2(&pipes[pipe].buffer, eof, 2);
    }
    else
    {
        pipes[pipe].reader_pid = -1;
    }

    if(pipes[pipe].writer_pid == -1 && pipes[pipe].reader_pid == -1) {
        pipes[pipe].available = 0;
        available_pipes[--current_available_pipe_index] = pipe;
    }
}