// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pipes.h"
#include "lib.h"
#include "IO.h"
#include "pqueue.h"
#include "semaphores.h"
#include "scheduler.h"
#include "tickets.h"

#define MUTEX_BASE MAX_PROCESS_BLOCKS
#define MAX_PIPES MAX_PROCESS_BLOCKS

typedef struct
{
    pqueue_t buffer;
    char available;
    int64_t blocked_pid;
    int64_t writer_pid;
    int64_t reader_pid;
    sem_t mutex;
} pipe_t;


pipe_t pipes[MAX_PIPES] = { 0 };
Tickets tickets_pipe = {0};

// ---------- Funciones Auxiliares -------

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
        return -1;
    }
    return (fd - 3) / 2;
}

static int8_t is_read_end(fd_t fd)
{
    return fd % 2 == 1; 
}

void initialize_pipes() 
{
    initialize_tickets(&tickets_pipe, pipes, sizeof(pipe_t), MAX_PIPES);
}

void p_unblock(int pipe)
{
    if (pipes[pipe].blocked_pid != -1)
    {
        unblock(pipes[pipe].blocked_pid);
        pipes[pipe].blocked_pid = -1;
    }
}


char pipe_is_valid(int pipe)
{
    return pipe >=0 && pipe < MAX_PIPES && pipes[pipe].available;
}

// -------------------- Funciones a exportar --------------------

int8_t create_pipe(fd_t* fd_buffer)
{
    int64_t pipe = request_ticket(&tickets_pipe);
    if (pipe >= MAX_PIPES || pipe < 0 || pipes[pipe].available == 1) 
    {
        return 0;
    }
    pipes[pipe].mutex = MUTEX_BASE + (sem_t)pipe;
    sem_open(pipes[pipe].mutex, 1);
    
    init_pqueue(&pipes[pipe].buffer);
    pipes[pipe].blocked_pid = -1;
    pipes[pipe].reader_pid = -1;
    pipes[pipe].writer_pid = -1;

    pipes[pipe].available = 1;
    pipe_to_fd(pipe, fd_buffer);
    return 1;
}

void assign_pipe_to_process(fd_t fd, int pid)
{
    int64_t pipe = fd_to_pipe(fd);

    if (!pipe_is_valid(pipe)) {
        // printf_error("[kernel] Wrong file descriptor [%d]. Pipe ID: %d \n", fd, pipe);
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
            // printf_error("File descriptor [%d] is already occupied\n", fd);
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
            // printf_error("File descriptor [%d] is already occupied\n", fd);
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
        p_enqueue(&pipes[pipe].buffer, eof[0]);
        p_enqueue(&pipes[pipe].buffer, eof[1]);
        p_unblock(pipe);
    }
    else
    {
        pipes[pipe].reader_pid = -1;
    }

    if(pipes[pipe].writer_pid == -1 && pipes[pipe].reader_pid == -1) 
    {
        pipes[pipe].available = 0;
        free_ticket(&tickets_pipe, pipe);
    }
}

int64_t read_pipe(fd_t fd_write_end, char* buf, int count)
{
    int64_t pipe = fd_to_pipe(fd_write_end);

    if (!pipe_is_valid(pipe) && p_is_empty(&pipes[pipe].buffer)) {
        buf[0] = EOF;
        buf[1] = '\0';
        return 0;
    }

    if (is_read_end(fd_write_end))
    {
        // printf_error("Cant read from the read-end part of a pipe of file descriptor %d. Pipe ID: [%d]\n", fd, pipe);
        return 0;
    }

    if (pipes[pipe].reader_pid != get_pid())
    {
        // printf_error("Cant read from this pid [%d]. Pipe ID: %d, File Descriptor: %d\n", get_pid(), pipe, fd);
        return 0;
    }
    
    uint64_t has_read;
    uint64_t total_read = 0;
    do
    {
        sem_wait(pipes[pipe].mutex);
        char to_read = 0;
        has_read = p_dequeue(&pipes[pipe].buffer, &to_read); // si no lee nada, devuelve 0
        buf[total_read] = to_read;
        total_read += has_read;

        // printf_error("lectura exitosa. Leido: [%d]. Falta [%d]\n", total_read, count - total_read);

        if(!has_read)
        {
            p_unblock(pipe);
            // printf_error("buffer vacio\n");
            int64_t current_pid = get_pid();
            pipes[pipe].blocked_pid = current_pid;
            sem_post(pipes[pipe].mutex);
            block(current_pid);
        }
        else
        {
            if(to_read == EOF || to_read == '\n') 
            {
                // printf_error("EOF or NEW_LINE detected!\n");
                sem_post(pipes[pipe].mutex);
                if(to_read == EOF) close_pipe_end(fd_write_end);
                return total_read;
            }
            sem_post(pipes[pipe].mutex);
        }
    } while(total_read < count);

    p_unblock(pipe);
    // printf_error("lectura exitosa. Leido: [%d]\n", to_return);

    return total_read;
}



// Requires a  of pipe = fd impar
int64_t write_pipe(fd_t fd_read_end, const char* buf, int count)
{
    int64_t pipe = fd_to_pipe(fd_read_end);

    if (!pipe_is_valid(pipe)) {
        // printf_error("[kernel] cant write to a closed pipe [%d]%d \n", fd, pipe);
        return 0;
    }

    if (!is_read_end(fd_read_end))
    {
        // printf_error("Cant write to the write-end part of a pipe of file descriptor %d. Pipe ID: [%d]\n", fd, pipe);
        return 0;
    }

    if (pipes[pipe].writer_pid != get_pid())
    {
        // printf_error("Cant write from this pid [%d]. Should write from pid [%d] instead. Pipe ID: %d, File Descriptor: %d '%s'\n", get_pid(), pipes[pipe].reader_pid, pipe, fd, buf[0] == -1 ?"EOF":"N");
        return 0;
    }
    
    uint64_t has_written;
    uint64_t total_written = 0;
    do
    {
        sem_wait(pipes[pipe].mutex);
        char to_write = buf[total_written];
        has_written = p_enqueue(&pipes[pipe].buffer, to_write);
        total_written += has_written;
        
        if(!has_written)
        {
            p_unblock(pipe);
            int64_t current_pid = get_pid();
            pipes[pipe].blocked_pid = current_pid;
            sem_post(pipes[pipe].mutex);
            block(current_pid);

        }
        else
        {
            if(to_write == EOF || to_write == '\n') 
            {
                sem_post(pipes[pipe].mutex);
                if(to_write == EOF) close_pipe_end(fd_read_end);
                return total_written;
            }
            sem_post(pipes[pipe].mutex);
        }
    } while(total_written < count);

    p_unblock(pipe);

    return total_written;
}
