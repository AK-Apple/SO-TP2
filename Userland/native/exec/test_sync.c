#include <stdint.h>
#include <stddef.h>
#include "../include/syscalls.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "test_util.h"

#define SEM_ID 5
#define TOTAL_PAIR_PROCESSES 2

int64_t global_shared_memory;

void slowInc(int64_t *p, int64_t inc) {
  int64_t aux = *p;
  sys_yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 5)
    return -1;

  if ((n = satoi(argv[1])) <= 0)
    return -1;
  if ((inc = satoi(argv[2])) == 0)
    return -1;
  if ((use_sem = satoi(argv[3])) < 0)
    return -1;
  if (use_sem)
    if (sys_sem_open(SEM_ID, 1) == SEM_ERROR) {
      printf_error("test_sync: ERROR opening semaphore\n");
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem) {
      if(sys_sem_wait(SEM_ID) == SEM_ERROR) {
        printf_error("test_sync: ERROR sem_wait with pid=%d\n", sys_get_pid());
        return -1;
      }
    }
    slowInc(&global_shared_memory, inc);
    if (use_sem) {
      if(sys_sem_post(SEM_ID) == SEM_ERROR) {
        printf_error("test_sync: ERROR sem_post with pid=%d\n", sys_get_pid());
        return -1;
      }
    }
  }

  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES] = {0};
  int64_t use_sem = 0;
  if (argc < 3) {
    printf_error("needs 2 arguments.\ntest_sync <n> <use_sem>\n");
    return -1;
  }
  if ((use_sem = satoi(argv[2])) < 0) {
    printf_error("use sem invalid argument %s\n", argv[2]);
    return -1;
  }

  char *argvDec[] = {"my_process_dec", argv[1], "-1", argv[2], NULL};
  char *argvInc[] = {"my_process_inc", argv[1], "1", argv[2], NULL};

  global_shared_memory = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = sys_create_process(my_process_inc, sizeof(argvDec)/sizeof(argvDec[0]), argvDec);
    pids[i + TOTAL_PAIR_PROCESSES] = sys_create_process(my_process_inc, sizeof(argvInc)/sizeof(argvInc[0]), argvInc);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    sys_wait_pid(pids[i]);
    sys_wait_pid(pids[i + TOTAL_PAIR_PROCESSES]);
  }
  if (use_sem)
    sys_sem_close(SEM_ID);
  printf("Final value: %d\n", global_shared_memory);

  return 0;
}

Program get_test_sync()
{
  return test_sync;
}