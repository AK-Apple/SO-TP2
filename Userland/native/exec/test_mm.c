// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stddef.h>
#include "command.h"
#include "stdio.h"
#include "stdlib.h"
#include "test_util.h"
#include "syscalls.h"
#include "shell.h"

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

int64_t test_mm(uint64_t argc, char *argv[]) 
{
  mm_rq mm_rqs[MAX_BLOCKS] = {0};
  uint8_t rq;
  uint32_t total;
  int use_smart_allocation = 1;
  Memory_Info meminfo = {0};
  sys_memory_info(&meminfo);
  int64_t max_memory = meminfo.total_memory;
  const uint64_t header_size = meminfo.header_size;

  if (argc >= 2) 
  {
    if ((max_memory = satoi(argv[1])) <= 0) 
    {
        max_memory = meminfo.total_memory;
        printf_error("numero de memoria maxima invalido: '%s'. usando valor por defecto=%d\n", argv[1], max_memory);
        return -1;
    }
    if(argc >= 3)
    {
        use_smart_allocation = (satoi(argv[2]) == 1);
    }
  }

  while (1) 
  {
    rq = 0;
    total = 0;
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) 
    {
      if(use_smart_allocation) 
      {
        sys_memory_info(&meminfo);
        uint64_t largest_free_block = meminfo.largest_free_block;
        if(largest_free_block <= header_size) break;
        uint64_t size = max_memory < largest_free_block ? max_memory : (largest_free_block - header_size); 
        mm_rqs[rq].size = GetUniform(size - 1) + 1;
      }
      else 
      {
        mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      }
      mm_rqs[rq].address = sys_memory_alloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address) 
      {
        total += mm_rqs[rq].size;
        rq++;
      }
      else if(use_smart_allocation) 
      {
        printf_error("failed to allocate %d bytes (%d)\n", mm_rqs[rq].size, mm_rqs[rq].size + header_size);
        print_meminfo_cmd();
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

Program get_test_mman() 
{
    return test_mm;
}