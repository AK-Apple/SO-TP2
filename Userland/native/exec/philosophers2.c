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
// int64_t aux_sem_id = 1;
// int64_t philosopher_count = 0;
// PHILOSOPHER_STATE state[MAX_PHILOSOPHER_COUNT] = {0};
// PHILOSOPHER_STATE pids[MAX_PHILOSOPHER_COUNT] = {0};


// static int left_fork_sem_id(int i) 
// {
//     return mutex_sem_id + 2 + i ;
// }

// static int right_fork_sem_id(int i, int current_philosopher_count) 
// {
//     return left_fork_sem_id((i >= current_philosopher_count - 1) ?  0 : i + 1) ;
// }

// static void print_state() {

//     const char state_str[] = {'_', 'T', 'H', 'E'};
//     // const char state_str[] = {'_', '.', '.', 'E'};
//     for(int i = 0; i < philosopher_count; i++) {
//         printf("%c ", state_str[state[i]]);
//     }
//     putchar('\n');
// }

// static void take_forks(int i, int current_philosopher_count) {
//     // sys_sem_wait(mutex_sem_id);
//     state[i] = HUNGRY;
//     print_state();
//     // sys_sem_post(mutex_sem_id);
//     if (IS_ODD(i))
//     {
//         sys_sem_wait(right_fork_sem_id(i, current_philosopher_count));
//         sys_sem_wait(left_fork_sem_id(i));
//     }
//     else
//     {
//         sys_sem_wait(left_fork_sem_id(i));
//         sys_sem_wait(right_fork_sem_id(i, current_philosopher_count));
//     }
    
// }

// static void put_forks(int i, int current_philosopher_count) {
//     sys_sem_post(left_fork_sem_id(i));
//     sys_sem_post(right_fork_sem_id(i, current_philosopher_count));
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
//         sys_sem_wait(mutex_sem_id);
//         sys_sem_post(mutex_sem_id);
//         think(i);

//         if (i == philosopher_count - 2) sys_sem_wait(aux_sem_id);
//         current_philosopher_count = philosopher_count;

//         take_forks(i, current_philosopher_count);
//         eat(i);
//         put_forks(i, current_philosopher_count);
//         if (i == philosopher_count - 2) sys_sem_post(aux_sem_id);
//     }
//     return 0;
// }


// static void add_philosopher() {

//     int i = philosopher_count;
    
//     if (i >= 4) 
//     {
//         sys_sem_wait(left_fork_sem_id(0));
//     }
//     state[i] = INVALID;
//     sys_sem_wait(mutex_sem_id);
    
//     if(i >= MAX_PHILOSOPHER_COUNT) {
//         sys_sem_post(mutex_sem_id);

//         if (i >= 4) sys_sem_post(left_fork_sem_id(0));

//         printf_error("max phi count\n");
//         return;
//     }
//     if(sys_sem_open(left_fork_sem_id(i), 1) == SEM_ERROR) {
//         sys_sem_post(mutex_sem_id);
        
//         if (i >= 4) sys_sem_post(left_fork_sem_id(0));

        
//         printf_error("error\n");
//         return;
//     }
//     char philosopher_i_str[PHILOSOPHER_I_STR_MAX] = {0};
//     itoa(i, philosopher_i_str, 10);
//     char *argv_phi[] = {"philosopher", philosopher_i_str};

//     philosopher_count++;

//     pids[i] = sys_create_process((Program)philosopher, sizeof(argv_phi)/sizeof(argv_phi[0]), argv_phi);
//     if(pids[i] == -1) {
//         sys_sem_post(mutex_sem_id);
//         philosopher_count--;
//         if (i >= 4) sys_sem_post(left_fork_sem_id(0));
        
//         return;
//     }
    
//     print_state();

//     if (i >= 4) sys_sem_post(left_fork_sem_id(0));

//     sys_sem_post(mutex_sem_id);

// }

// static void remove_philosopher() {
//     if (philosopher_count <= 4)
//     {
//         printf_error("You can have minimum 4 philosophers\n");
//         return;
//     }
//     int i = philosopher_count - 1;

//     sys_sem_wait(aux_sem_id);
//     take_forks(i, philosopher_count);

//     sys_sem_wait(mutex_sem_id);
//     // if left && right == hungry

//     sys_kill_process(pids[i], 0);
//     sys_wait_pid(pids[i]);

//     pids[i] = -1;
//     state[i] = INVALID;
    
//     print_state();
    
//     sys_sem_post(aux_sem_id);
//     sys_sem_post(left_fork_sem_id(i));
//     sys_sem_close(left_fork_sem_id(i));
//     sys_sem_post(right_fork_sem_id(i, philosopher_count));

//     philosopher_count--;
    
//     sys_sem_post(mutex_sem_id);
// }


// int64_t phylo(uint64_t argc, char *argv[]) {
//     mutex_sem_id = 0;       // Funciona como ID base para el resto de semáforos
//     aux_sem_id = mutex_sem_id + 1;
//     philosopher_count = 0;
//     mutex_sem_id = sys_get_pid();
//     if(sys_sem_open(mutex_sem_id, 1) == SEM_ERROR) {
//         printf_error("phylo error: init sem\n");
//         return -1;
//     }
//     if(sys_sem_open(aux_sem_id, 1) == SEM_ERROR) {
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
//             sys_print_all_processes();
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