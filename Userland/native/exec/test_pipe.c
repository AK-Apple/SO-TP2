#include "syscalls.h"
#include "stdio.h"
#include "command.h"
#include <stdint.h>

int64_t read_test(uint64_t argc, char *argv[])  {
    char buffer[20] = { 0 };
    sys_read(0, buffer, 10);
    sys_write(1, buffer, 10);
    sys_read(0, buffer, 8);
    sys_write(1, buffer, 8);
    putchar('\n');
    sys_read(0, buffer, 1);
    sys_write(1, buffer, 1);

    putchar('\n');
    sys_read(0, buffer, 4);
    sys_write(1, buffer, 4);

    return 0;
}

int64_t write_test(uint64_t argc, char *argv[])  {
    sys_write(1, "123456789\n", 10);
    sys_write(1, "987654210", 9);
    sys_write(1, "ABC", 3);
    sys_sleep(2);
    sys_write(1, "D", 1);

    // printf("9876543");
    return 0;
}

int64_t test_pipe(uint64_t argc, char *argv[]) {
    printf("TEST DE PIPES!!\n");

    int64_t test_pipe_fds[2] = { 0 };

    sys_create_pipe(test_pipe_fds);

    int fds1[] = {test_pipe_fds[1], STD_OUT, STD_ERR};
    int pid1 = sys_create_process_fd(read_test, 0, 0, fds1);

    int fds2[] = {STD_IN, test_pipe_fds[0], STD_ERR};
    int pid2 = sys_create_process_fd(write_test, 0, 0, fds2);
    sys_wait_pid(pid1);
    sys_wait_pid(pid2);
    printf("pid read_test: %d\npid write_test: %d\n", pid1, pid2);

    sys_close_pipe(test_pipe_fds[0]);
    char *argv_echo[] = {"echo", "arg1", "arg2", "arg3", "arg4"};

    sys_create_pipe(test_pipe_fds);

    pid1 = sys_create_process_fd((Program)cat, 0, 0, fds1);
    pid2 = sys_create_process_fd(echo_cmd, sizeof(argv_echo)/sizeof(argv_echo[0]), argv_echo, fds2);
    sys_wait_pid(pid1);
    sys_wait_pid(pid2);
    sys_close_pipe(test_pipe_fds[0]);
    return 0;
}
