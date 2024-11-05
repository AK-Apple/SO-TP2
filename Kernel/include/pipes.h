#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>



int8_t create_pipe(int64_t* fd_buffer);
int64_t read_pipe(int fd, char* buf, int count);
int64_t write_pipe(int fd, const char* buf, int count);
int8_t close_pipe(int fd);
void assign_pipe_to_process(int fd, int pid);
void close_pipe_end(int fd);

#endif // PIPES_H