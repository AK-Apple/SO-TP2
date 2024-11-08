#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>
#include "shared.h"


int8_t create_pipe(fd_t* fd_buffer);
int64_t read_pipe(fd_t fd, char* buf, int count);
int64_t write_pipe(fd_t fd, const char* buf, int count);
void assign_pipe_to_process(fd_t fd, int pid);
void close_pipe_end(fd_t fd);
uint8_t pipe_is_closable(fd_t fd);

#endif // PIPES_H