#include "fl_allocator.h"
#include "IO.h"
#include "lib.h"
#include <stdint.h>

#define MAX_BLOCKS BLOCK_COUNT

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint8_t memcheck(void *start, uint8_t value, uint32_t size) {
  uint8_t *p = (uint8_t *)start;
  uint32_t i;

  for (i = 0; i < size; i++, p++)
    if (*p != value)
      return 0;

  return 1;
}

uint64_t test_mm() {
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

//   if (argc != 1)
//     return -1;

  // if ((max_memory = satoi(argv[0])) <= 0)
    // return -1;

  max_memory = fl_total_memory();
  
  int test_iterations = 2048;
  while (test_iterations--) {
    rq = 0;
    printf("memory manager cycle\n"); 
    total = 0;

    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
    //   mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
    //   mm_rqs[rq].address = fl_malloc(mm_rqs[rq].size);
      mm_rqs[rq].size = BLOCK_SIZE;
      mm_rqs[rq].address = fl_malloc();
      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
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
          printf("test_mm ERROR\n");
          return -1;
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        fl_free(mm_rqs[i].address);
  }
}