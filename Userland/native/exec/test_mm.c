#include <stdint.h>
#include <stddef.h>
#include "../include/command.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/test_util.h"
#include "../include/syscalls.h"

#define MAX_BLOCKS 4096
#define HEADER_SIZE 32

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {
  static mm_rq mm_rqs[MAX_BLOCKS] = {0};
  uint8_t rq;
  uint32_t total;
  int64_t max_memory = (1L << 20)-HEADER_SIZE*256;
  int use_smart_allocation = 1;

  if (argc >= 2) {
    if ((max_memory = satoi(argv[1])) <= 0) {
        printf_error("numero invalido: '%s'\n", argv[1]);
        return -1;
    }
    if(argc >= 3) {
        use_smart_allocation = (satoi(argv[2]) == 1);
    }
  }

  while (1) {
    rq = 0;
    total = 0;
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      if(use_smart_allocation) {
        uint64_t largest_free_block = sys_memory_largest_block();
        if(largest_free_block <= HEADER_SIZE) break;
        mm_rqs[rq].size = GetUniform(largest_free_block - (HEADER_SIZE-2) - 1) + 1;
      }
      else {
        mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      }
      mm_rqs[rq].address = sys_memory_alloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        // printf("allocate %d\n", mm_rqs[rq].size);
        rq++;
      }
      else {
        printf_error("failed to allocate %d bytes (%d)\n", mm_rqs[rq].size, mm_rqs[rq].size + HEADER_SIZE);
        sys_memory_info();
      }
    }
    uint32_t i;

    // // Set
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);

    // // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf_error("test_mm ERROR\n");
          return -1;
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        sys_memory_free(mm_rqs[i].address);
  }
}

Program get_test_mman() {
    return test_mm;
}