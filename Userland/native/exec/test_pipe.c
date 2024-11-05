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

    fd_t test_pipe_fds[2] = { 0 };

    sys_create_pipe(test_pipe_fds);

    fd_t fds1[] = {test_pipe_fds[1], STDOUT, STDERR};
    pid_t pid1 = sys_create_process_fd(read_test, 0, 0, fds1);

    fd_t fds2[] = {STDIN, test_pipe_fds[0], STDERR};
    pid_t pid2 = sys_create_process_fd(write_test, 0, 0, fds2);
    sys_wait_pid(pid1);
    sys_wait_pid(pid2);
    printf("pid read_test: %ld\npid write_test: %ld\n", pid1, pid2);

    char *argv_echo[] = {"echo", "arg1", "arg2", "arg3", "arg4"};

    sys_create_pipe(test_pipe_fds);

    pid1 = sys_create_process_fd((Program)cat, 0, 0, fds1);
    pid2 = sys_create_process_fd(echo_cmd, sizeof(argv_echo)/sizeof(argv_echo[0]), argv_echo, fds2);
    sys_wait_pid(pid1);
    sys_wait_pid(pid2);
    return 0;
}
