//
// Created by uakruk on 4/5/16.
//

#ifndef OCP_ALLOCATOR_H
#define OCP_ALLOCATOR_H

#include <stddef.h>

struct data {
};

struct block_header {
    size_t size;
    // the Nth bit of size 'll be used as free or not flag.
    // for 32-bit systems maximum size of the block will be 2 GB
    // for 64-bit much more
    // 1 - busy
    // 0 - free
    size_t previous_size;
};

struct block {
    struct block_header header;

};

extern size_t mem_size;
extern void * head;
extern struct block * last_free;
const unsigned int HEADER_OFFSET = sizeof(block);
const size_t busy = (size_t)0x0000000000000001 << (8 * sizeof(size_t) -1);
//void * data_array = NULL;

void init(void *init_addr, size_t mem_size);
void * mem_alloc(size_t size);
void * mem_realloc(void *addr, size_t size);
void mem_free(void *addr);


//class Allocator {
//};


#endif //OCP_ALLOCATOR_H
