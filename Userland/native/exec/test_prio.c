// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include "../include/syscalls.h"
#include "../include/test_util.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/command.h"
#define MINOR_WAIT 10000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 100000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  // TODO: Change as required
#define MEDIUM 3  // TODO: Change as required
#define HIGHEST 5 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {

  int64_t pids[TOTAL_PROCESSES];
  char *loop_argv[] = {"endless_loop_print", "20000000"};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = sys_create_process(get_endless_loop_print(), sizeof(loop_argv)/sizeof(loop_argv[0]), loop_argv);

  printf("\nBUSY WAIT INCOMING...\n");

  bussy_wait(WAIT);
  
  printf("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_change_priority(pids[i], prio[i]);

  for(int i = 0; i < 5; i++) {
    bussy_wait(WAIT);
  }
  
  printf("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_block(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_change_priority(pids[i], MEDIUM);

  printf("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_unblock(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_kill_process(pids[i]);

}

// 3 iiiiiiiiiiiiiiii
// 4 iiiiiiiiiiiiiiii
// 5 iiiiiiiiiiiiiiii