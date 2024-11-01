#include "syscalls.h"
#include "stdio.h"
#include "command.h"
#include <stdint.h>

#define TEST_PIPE_FD 7

int64_t read_test(uint64_t argc, char *argv[])  {
    char buffer[20] = { 0 };
    sys_read(0, buffer, 10);
    sys_write(1, buffer, 10);
    return 0;
}

int64_t write_test(uint64_t argc, char *argv[])  {
    sys_write(1, "123456789\n", 10);
    return 0;
}

int64_t test_pipe(uint64_t argc, char *argv[]) {
    printf("TEST DE PIPES!!");


    sys_create_pipe(TEST_PIPE_FD);
    int fds1[] = {TEST_PIPE_FD, STD_OUT, STD_ERR};
    sys_create_process_fd(read_test, 0, 0, fds1);

    int fds2[] = {STD_IN, TEST_PIPE_FD, STD_ERR};
    sys_create_process_fd(write_test, 0, 0, fds2);
    return 0;
}
