// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include "syscalls.h"
#include "test_util.h"
#include "stdio.h"
#include "stdlib.h"
#include "command.h"
#include "shared.h"
#include "string.h"
#define WAIT 400000000

#define TOTAL_PROCESSES 3
#define LOWEST PRIORITY_LOW  
#define MEDIUM PRIORITY_MID  
#define HIGHEST PRIORITY_HIGH 

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

int64_t test_prio(uint64_t argc, char *argv[]) 
{
  int64_t pids[TOTAL_PROCESSES] = {0};
  char *print_color[] = {"255", "65280", "16711680"};
  char *loop_argv[] = {"endless_loop_print", "17000000", ""};
  uint64_t i;
  if(argc >= 2 && is_integer(argv[1])) 
  {
    loop_argv[1] = argv[1];
  }

  for (i = 0; i < TOTAL_PROCESSES; i++) 
  {
    loop_argv[2] = print_color[i % (sizeof(print_color) / sizeof(print_color[0]))];
    pids[i] = sys_create_process((Program)endless_loop_print, sizeof(loop_argv)/sizeof(loop_argv[0]), loop_argv);
    if(pids[i] == INVALID_PID)
      return -1;
  }

  printf("\nBUSY WAIT INCOMING...\n");

  bussy_wait(WAIT);
  
  printf("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_change_priority(pids[i], prio[i]);
  sys_yield();
  print_processes_state();
  for(int i = 0; i < 5; i++) 
  {
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
    sys_kill_process(pids[i], 0);

  return 0;
}
