// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// #include "stdio.h"
// #include "command.h"
// #include "syscalls.h"
// #include "stdlib.h"
// #include <stdint.h>


// #define PHILOSOPHER_I_STR_MAX 16
// #define SECONDS_THINK 3
// #define SECONDS_EAT 2
// #define MAX_PHILOSOPHER_COUNT 16
// #define MIN_PHILOSOPHER_COUNT 3
// #define INI_PHILOSOPHER_COUNT 5

// #define IS_ODD(n) ((n)%2 != 0)

// #define CMD_ADD 'a'
// #define CMD_REMOVE 'r'
// #define CMD_PS 'p'
// #define CMD_CLEAR 'c'

// typedef enum {
//     INVALID = 0,
//     THINKING,
//     HUNGRY,
//     EATING,
// } PHILOSOPHER_STATE;

// int64_t mutex_sem_id = 0;
// int64_t add_philosopher_sem_id = 1;
// int64_t rm_philosopher_sem_id = 2;
// int64_t philosopher_count = 0;
// PHILOSOPHER_STATE state[MAX_PHILOSOPHER_COUNT] = {0};
// PHILOSOPHER_STATE pids[MAX_PHILOSOPHER_COUNT] = {0};


// static int left_fork_sem_id(int i) 
// {
//     return mutex_sem_id + 2 + i ;
// }

// static int right_fork_sem_id(int i) 
// {
//     return left_fork_sem_id((i >= philosopher_count - 1) ?  0 : i + 1) ;
// }

// static void print_state() {

//     const char state_str[] = {'_', 'T', 'H', 'E'};
//     // const char state_str[] = {'_', '.', '.', 'E'};
//     for(int i = 0; i < philosopher_count; i++) {
//         printf("%c ", state_str[state[i]]);
//     }
//     putchar('\n');
// }

// static void take_forks(int i) {
//     // sys_sem_wait(mutex_sem_id);
//     state[i] = HUNGRY;
//     print_state();
//     // sys_sem_post(mutex_sem_id);
//     if (IS_ODD(i))
//     {
//         sys_sem_wait(right_fork_sem_id(i));
//         sys_sem_wait(left_fork_sem_id(i));
//     }
//     else
//     {
//         sys_sem_wait(left_fork_sem_id(i));
//         sys_sem_wait(right_fork_sem_id(i));
//     }
    
// }

// static void put_forks(int i) {
//     sys_sem_post(left_fork_sem_id(i));
//     sys_sem_post(right_fork_sem_id(i));
// }

// static void eat(int i)
// {
//     // sys_sem_wait(mutex_sem_id);
//     state[i] = EATING;
//     print_state();
//     // sys_sem_post(mutex_sem_id);
//     sys_sleep(SECONDS_EAT);
// }

// static void think(int i)
// {
//     // sys_sem_wait(mutex_sem_id);
//     state[i] = THINKING;
//     print_state();
//     // sys_sem_post(mutex_sem_id);
//     sys_sleep(SECONDS_THINK);
// }

// static int64_t philosopher(uint64_t argc, char *argv[]) {
//     while (philosopher_count < 4) sys_yield();
//     int i = atoi(argv[1]);
//     int current_philosopher_count = 0;
//     printf("Empieza el filosofo numero %d\n", i);
//     while(1) {

//         think(i);

//         if (i == philosopher_count - 1)
//         {
//             sys_sem_wait(add_philosopher_sem_id);
//             sys_sem_wait(rm_philosopher_sem_id);
//             take_forks(i);
//             eat(i);
//             put_forks(i);
//             sys_sem_post(rm_philosopher_sem_id);
//             sys_sem_post(add_philosopher_sem_id);
//         }
//         if (i == philosopher_count -2)
//         {
//             sys_sem_wait(rm_philosopher_sem_id);
//             take_forks(i);
//             eat(i);
//             put_forks(i);
//             sys_sem_post(rm_philosopher_sem_id);
//         }
//         else{
//             take_forks(i);
//             eat(i);
//             put_forks(i);
//         }
//     }
//     return 0;
// }


// static void add_philosopher() {

//     int i = philosopher_count;

//     state[i] = INVALID;
//     sys_sem_wait(mutex_sem_id);
    
//     if(i >= MAX_PHILOSOPHER_COUNT) {
//         sys_sem_post(mutex_sem_id);
//         printf_error("max phi count\n");
//         return;
//     }
//     if(sys_sem_open(left_fork_sem_id(i), 1) == SEM_ERROR) {
//         sys_sem_post(mutex_sem_id);
//         printf_error("error\n");
//         return;
//     }
//     char philosopher_i_str[PHILOSOPHER_I_STR_MAX] = {0};
//     itoa(i, philosopher_i_str, 10);
//     char *argv_phi[] = {"philosopher", philosopher_i_str};

//     sys_sem_wait(add_philosopher_sem_id);

//     philosopher_count++;

//     pids[i] = sys_create_process((Program)philosopher, sizeof(argv_phi)/sizeof(argv_phi[0]), argv_phi);
//     if(pids[i] == -1) {
//         sys_sem_post(mutex_sem_id);
//         sys_sem_post(add_philosopher_sem_id);
//         philosopher_count--;
        
//         return;
//     }
    
//     print_state();
//     sys_sem_post(add_philosopher_sem_id);
//     sys_sem_post(mutex_sem_id);

// }

// static void remove_philosopher() {

//     int i = philosopher_count - 1;

//     sys_sem_wait(mutex_sem_id);

//     sys_sem_wait(rm_philosopher_sem_id);
//     sys_sem_wait(rm_philosopher_sem_id);


//     sys_kill_process(pids[i], 0);

//     pids[i] = -1;
//     state[i] = INVALID;
    
//     print_state();
    
//     sys_sem_post(rm_philosopher_sem_id);
//     sys_sem_post(rm_philosopher_sem_id);

//     sys_sem_post(left_fork_sem_id(i));
//     sys_sem_close(left_fork_sem_id(i));

//     philosopher_count--;
    
//     sys_sem_post(mutex_sem_id);
// }


// int64_t phylo(uint64_t argc, char *argv[]) {
//     mutex_sem_id = 0;       // Funciona como ID base para el resto de semáforos
//     add_philosopher_sem_id = mutex_sem_id + 1;
//     rm_philosopher_sem_id = mutex_sem_id + 2;

//     philosopher_count = 0;
//     mutex_sem_id = sys_get_pid();
//     if(sys_sem_open(mutex_sem_id, 1) == SEM_ERROR) {
//         printf_error("phylo error: init sem\n");
//         return -1;
//     }
//     if(sys_sem_open(add_philosopher_sem_id, 1) == SEM_ERROR) {
//         printf_error("phylo error: init sem 2\n");
//         return -1;
//     }
//     if(sys_sem_open(rm_philosopher_sem_id, 2) == SEM_ERROR) {
//         printf_error("phylo error: init sem 2\n");
//         return -1;
//     }

//     for(int i = 0; i < INI_PHILOSOPHER_COUNT; i++) {
//         add_philosopher();
//     }
//     int cmd = 0;
//     while((cmd = getchar()) != EOF) {
//         switch (cmd)
//         {
//         case CMD_ADD:
//             add_philosopher();
//             break;
//         case CMD_REMOVE:
//             if(philosopher_count > MIN_PHILOSOPHER_COUNT)
//                 remove_philosopher();
//             else 
//                 printf_error("min phi count\n");
//             break;
//         case CMD_PS:
//             print_processes_state();
//             break;
//         case CMD_CLEAR:
//             sys_clear();
//             break;
//         default:
//             break;
//         }
//     }
//     printf("Echando a los filosofos\n");
//     while(philosopher_count > 0) {
//         remove_philosopher();
//     }
//     sys_sem_close(mutex_sem_id);

//     return 0;
// }