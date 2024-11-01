#include "stdio.h"
#include "command.h"
#include "syscalls.h"
#include "stdlib.h"
#include <stdint.h>

#define PHILOSOPHER_I_STR_MAX 16
#define SECONDS_THINK 3
#define SECONDS_EAT 2
#define MAX_PHILOSOPHER_COUNT 16
#define MIN_PHILOSOPHER_COUNT 3
#define INI_PHILOSOPHER_COUNT 5
#define CMD_ADD 'a'
#define CMD_REMOVE 'r'
#define CMD_PS 'p'
#define CMD_CLEAR 'c'
typedef enum {
    INVALID = 0,
    THINKING,
    HUNGRY,
    EATING,
} PHILOSOPHER_STATE;

int64_t mutex_sem_id = 0;
int64_t philosopher_count = 0;
PHILOSOPHER_STATE state[MAX_PHILOSOPHER_COUNT] = {0};
PHILOSOPHER_STATE pids[MAX_PHILOSOPHER_COUNT] = {0};

static int philosopher_sem_id(int i) {
    return mutex_sem_id + 1 + i;
}

static int left(int i) {
    return (i + philosopher_count - 1) % philosopher_count;
}

static int right(int i) {
    return (i + 1) % philosopher_count;
}

static void print_state() {
    const char state_str[] = {'_', 'T', 'H', 'E'};
    // const char state_str[] = {'_', '.', '.', 'E'};
    for(int i = 0; i < philosopher_count; i++) {
        printf("%c ", state_str[state[i]]);
    }
    putchar('\n');
}

static void test(int i) {
    if(state[i] == HUNGRY && state[left(i)] != EATING && state[right(i)] != EATING) {
        state[i] = EATING;
        print_state();
        sys_sem_post(philosopher_sem_id(i));
    }
}

static void take_forks(int i) {
    sys_sem_wait(mutex_sem_id);
    state[i] = HUNGRY;
    // print_state();
    test(i);
    sys_sem_post(mutex_sem_id);
    sys_sem_wait(philosopher_sem_id(i));
}

static void put_forks(int i) {
    sys_sem_wait(mutex_sem_id);
    state[i] = THINKING;
    print_state();
    test(left(i));
    test(right(i));
    sys_sem_post(mutex_sem_id);
}

static int64_t philosopher(uint64_t argc, char *argv[]) {
    int i = atoi(argv[1]);
    printf("Empieza el filosofo numero %d\n", i);
    state[i] = THINKING;
    while(1) {
        sys_sleep(SECONDS_THINK);
        take_forks(i);
        sys_sleep(SECONDS_EAT);
        put_forks(i);
    }
    return 0;
}

static void add_philosopher() {
    sys_sem_wait(mutex_sem_id);
    int i = philosopher_count;
    if(i >= MAX_PHILOSOPHER_COUNT) {
        printf_error("max phi count\n");
        return;
    }
    if(sys_sem_open(philosopher_sem_id(i), 0) == SEM_ERROR) {
        printf_error("error\n");
        return;
    }
    char philosopher_i_str[PHILOSOPHER_I_STR_MAX] = {0};
    itoa(i, philosopher_i_str, 10);
    char *argv_phi[] = {"philosopher", philosopher_i_str};
    pids[i] = sys_create_process((Program)philosopher, sizeof(argv_phi)/sizeof(argv_phi[0]), argv_phi);
    if(pids[i] == -1)
        return;
    philosopher_count++;
    print_state();
    sys_sem_post(mutex_sem_id);
}

static void remove_philosopher() {
    int i = philosopher_count - 1;
    sys_sem_wait(mutex_sem_id);
    while(state[left(i)] == EATING && state[right(i)] == EATING) {
        sys_sem_post(mutex_sem_id);
        sys_sem_wait(philosopher_sem_id(i));
        sys_sem_wait(mutex_sem_id);
    }
    sys_kill_process(pids[i]);
    sys_wait_pid(pids[i]);
    sys_sem_close(philosopher_sem_id(i));
    pids[i] = -1;
    state[i] = INVALID;
    philosopher_count--;
    print_state();
    sys_sem_post(mutex_sem_id);

    
}

int64_t phylo(uint64_t argc, char *argv[]) {
    mutex_sem_id = sys_get_pid();
    if(sys_sem_open(mutex_sem_id, 1) == SEM_ERROR) {
        printf_error("phylo error: init sem\n");
        return -1;
    }

    for(int i = 0; i < INI_PHILOSOPHER_COUNT; i++) {
        add_philosopher();
    }
    int cmd = 0;
    while((cmd = getchar()) != EOF) {
        switch (cmd)
        {
        case CMD_ADD:
            add_philosopher();
            break;
        case CMD_REMOVE:
            if(philosopher_count > MIN_PHILOSOPHER_COUNT)
                remove_philosopher();
            else 
                printf_error("min phi count\n");
            break;
        case CMD_PS:
            sys_print_all_processes();
            break;
        case CMD_CLEAR:
            sys_clear();
            break;
        default:
            break;
        }
    }
    printf("Echando a los filosofos\n");
    while(philosopher_count > 0) {
        remove_philosopher();
    }
    sys_sem_close(mutex_sem_id);

    return 0;
}