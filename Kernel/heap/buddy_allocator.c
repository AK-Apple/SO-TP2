// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "memory_allocator.h"
#include "shared.h"
#include "IO.h"
#include "lib.h"
#include <stddef.h>
#include <stdint.h>

#ifdef USE_BUDDY_ALLOCATOR

#define BUCKET_COUNT 31
#define HEADER_SIZE_LOG2 5 

typedef struct BuddyHeader {
    struct BuddyHeader *prev;  // para el linked list de este buddy
    struct BuddyHeader *next;  
    uint64_t bucket_index;      // indice en bucket, tambien es el exponente de tamano del bloque
    uint64_t intended_size;     // tamano que se pidio originalmente
} BuddyHeader;

typedef struct BuddyAllocator {
    BuddyHeader *header_buckets[BUCKET_COUNT]; // vector de linked list
    void *base_address;
    uint64_t max_bucket_index;
} BuddyAllocator;
/*
bucket[0] -> 2^(0+4)=16 size blocks
bucket[1] -> 2^(1+4)=32 size blocks
bucket[12] -> 2^(12+4)=65536 size blocks
*/
BuddyAllocator allocator = {0};

/*
Dado una cantidad de bytes
calcula el indice del bucket mas pequeno cuyo tamano es >= bytes
*/
static size_t size_to_bucket_index(size_t bytes) 
{
  size_t bucket = 0;
  size_t size = sizeof(BuddyHeader);

  while (size < bytes) 
  {
    size *= 2;
    bucket++;
  }

  return bucket;
}

/*
proceso inverso de size_to_bucket_index
*/
static uint64_t index_to_size(size_t bucket_index) 
{
    return 1L << (bucket_index + HEADER_SIZE_LOG2);
}

static void make_BuddyHeader(BuddyHeader *header, uint64_t bucket_index, BuddyHeader *next) 
{
    header->bucket_index = bucket_index;
    header->intended_size = 0;
    header->prev = NULL;
    header->next = next;
    if(next != NULL) 
    {
        next->prev = header;
    }

    allocator.header_buckets[bucket_index] = header;
}

static void delete_BuddyHeader(BuddyHeader *header) 
{
    if(header->prev != NULL)
        header->prev->next = header->next;
    else
        allocator.header_buckets[header->bucket_index] = header->next;

    if(header->next != NULL)
        header->next->prev = header->prev;
}

static BuddyHeader *get_its_buddy(BuddyHeader *header) 
{
    uint64_t position = (uint64_t) header - (uint64_t)allocator.base_address;
    return (BuddyHeader *) ((uint64_t) allocator.base_address + (position ^ index_to_size(header->bucket_index)));
}

void initialize_memory_allocator(void *base_address, uint64_t total_bytes) 
{
    total_bytes = 1L << log(total_bytes, 2);
    base_address += sizeof(BuddyHeader) - (uint64_t)base_address % sizeof(BuddyHeader);
    uint64_t bucket_index = size_to_bucket_index(total_bytes);
    allocator.max_bucket_index = bucket_index;
    allocator.base_address = base_address;
    make_BuddyHeader((BuddyHeader *)base_address, bucket_index, NULL);
}

void *memory_alloc(size_t bytes) 
{
    uint64_t bucket_index = size_to_bucket_index(bytes + sizeof(BuddyHeader));
    if(!allocator.header_buckets[bucket_index]) 
    {
        uint64_t nearest_index = 0;
        for(uint64_t i = bucket_index + 1; i <= allocator.max_bucket_index && !nearest_index; i++) 
        {
            if(allocator.header_buckets[i]) 
            {
                nearest_index = i;
            }
        }
        if(!nearest_index) return NULL;
        while(bucket_index < nearest_index) 
        {
            BuddyHeader *original_header = allocator.header_buckets[nearest_index];
            delete_BuddyHeader(original_header);
            nearest_index--;
            BuddyHeader *header2 = (BuddyHeader *) ((uint64_t) original_header + index_to_size(nearest_index));
            make_BuddyHeader(header2, nearest_index, allocator.header_buckets[nearest_index]);
            make_BuddyHeader(original_header, nearest_index, header2);
        }
    }
    BuddyHeader *header = allocator.header_buckets[bucket_index];
    delete_BuddyHeader(header);
    header->intended_size = bytes;
    header->prev = NULL;
    header->next = NULL;

    return (void *)header + sizeof(BuddyHeader);
}

void memory_free(void *pointer) 
{
    BuddyHeader *header = (BuddyHeader *) (pointer - sizeof(BuddyHeader));
    header->intended_size = 0;

    BuddyHeader *last_address = allocator.base_address + index_to_size(allocator.max_bucket_index);
    BuddyHeader *BuddyHeader = get_its_buddy(header);
    while(BuddyHeader < last_address && !BuddyHeader->intended_size && BuddyHeader->bucket_index == header->bucket_index) 
    {
        delete_BuddyHeader(BuddyHeader);
        header = (header < BuddyHeader) ? header : BuddyHeader;
        header->bucket_index++;
        BuddyHeader = get_its_buddy(header);
    }
    make_BuddyHeader(header, header->bucket_index, allocator.header_buckets[header->bucket_index]);
}

void memory_info(Memory_Info *info) 
{
    uint64_t total_memory = index_to_size(allocator.max_bucket_index);
    uint64_t used_memory = 0;
    uint64_t internal_fragmentation = 0;
    void *current_pointer = allocator.base_address;
    uint64_t largest_free_block_bytes = 0;

    while(current_pointer - allocator.base_address < total_memory) 
    {
        BuddyHeader *header = (BuddyHeader *) current_pointer;
        uint64_t block_size = index_to_size(header->bucket_index);
        if(header->intended_size) 
        {
            // printf_color("|%lu:%lu", 0x00DDDDFF, block_size, header->intended_size);
            internal_fragmentation += block_size - header->intended_size - sizeof(BuddyHeader);
            used_memory += block_size;
        }
        else {
            // printf_color("|%lu:free", 0x00888888, block_size);
            if(largest_free_block_bytes < header->bucket_index) 
            {
                largest_free_block_bytes = header->bucket_index;
            }
        }
        current_pointer += block_size;
    }

    info->allocator_type = "buddy_binary";
    info->total_memory = total_memory;
    info->largest_free_block = index_to_size(largest_free_block_bytes);
    info->header_size = sizeof(BuddyHeader);
    info->base_address = (uint64_t)allocator.base_address;
    info->end_address = (uint64_t)allocator.base_address + total_memory - 1;
    info->used_memory = used_memory;
    info->internal_fragmentation = internal_fragmentation;
    info->free_memory = total_memory- used_memory;
}
#endif
