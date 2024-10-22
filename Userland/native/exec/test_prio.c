#include <stdint.h>
#include <stdio.h>
#include "../include/syscalls.h"
#include "../include/test_util.h"
#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 1  // TODO: Change as required
#define MEDIUM 3  // TODO: Change as required
#define HIGHEST 5 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {

  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {"endless_loop_print"};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = sys_create_process(get_endless_loop_print(), 1, argv);

  printf("\nBUSY WAIT INCOMING...\n");

for(int i = 0; i < 10; i++) {
  bussy_wait(WAIT);
}
  
  printf("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_change_priority(pids[i], prio[i]);

  bussy_wait(WAIT);
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

Program get_test_prio(){
  return (Program)test_prio;
}