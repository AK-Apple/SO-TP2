// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/stdio.h"
#include "../include/syscalls.h"
#include "../include/test_util.h"

enum State { RUNNING,
             BLOCKED,
             KILLED };

typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;



int64_t test_processes(uint64_t argc, char *argv[]) 
{
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  int64_t max_processes = -1;
  char *argvAux[] = {"endless_loop"};

  if (argc >= 2) {
    max_processes = satoi(argv[1]);
  }
  if(max_processes <= 0) 
  {
    max_processes = 30;
  }
  p_rq p_rqs[max_processes];

  while (1) 
  {

    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) 
    {
      p_rqs[rq].pid = sys_create_process((Program)endless_loop, 1, argvAux);

      if (p_rqs[rq].pid == -1) 
      {
        printf_error("test_processes: ERROR creating process\n");
        return -1;
      } else 
      {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {
      for (rq = 0; rq < max_processes; rq++) 
      {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) 
            {
              if (sys_kill_process(p_rqs[rq].pid, 0) == -1) 
              {
                printf_error("test_processes: ERROR killing process\n");
                return -1;
              }
              p_rqs[rq].state = KILLED;
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING) {
              if (sys_block(p_rqs[rq].pid) == -1) 
              {
                printf_error("test_processes: ERROR blocking process\n");
                return -1;
              }
              p_rqs[rq].state = BLOCKED;
            }
            break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) 
        {
          if (sys_unblock(p_rqs[rq].pid) == -1) 
          {
            printf_error("test_processes: ERROR unblocking process\n");
            return -1;
          }
          p_rqs[rq].state = RUNNING;
        }
    }
  }
}