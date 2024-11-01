#ifndef PIPES_H
#define PIPES_H

#include <stdint.h>



int8_t create_pipe(int fd);
int64_t read_pipe(int fd, char* buf, int count);
int64_t write_pipe(int fd, const char* buf, int count);
int64_t request_pipe();
int8_t close_pipe(int fd);

#endif // PIPES_H