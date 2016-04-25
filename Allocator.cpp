//
// Created by uakruk on 4/5/16.
//

#include "Allocator.h"
#include <iostream>
using namespace std;

size_t mem_size = 0;
void * head = NULL;
struct block * last_free = NULL;

bool is_free(struct block * block);


// initalization of memory allocator with address of allocating beginning and memory size
void init(void * init_addr, size_t init_size)
{
    head = init_addr;
    mem_size = init_size;
    struct block * init_block = (block *) head;
    init_block->header.previous_size = 0;
    init_block->header.size = (mem_size - sizeof(block_header));
  //  init_block->header.size = busy; // marking it as free
    last_free = init_block;
}

// check wheter it's not the end of allocating memory
bool has_next_block(struct block * ptr)
{
    bool resp = true;
    if (ptr + ptr->header.size + HEADER_OFFSET >= (head + mem_size))
        resp = false;
    return resp;
}

// check whether previous block is present
bool has_prev_block(struct block * ptr)
{
    return ptr->header.previous_size >= HEADER_OFFSET;
}

// assume that parent block is free
struct block * divide_block(struct block * parent, size_t size)
{
    if (parent->header.size - size < HEADER_OFFSET + 1)
        size = parent->header.size; // if there is no enough space to make new block with at least 1 byte of free space,
                                    // add leftover free memory to this block
    size_t first_size = size;
    size_t second_size = parent->header.size - size - HEADER_OFFSET;
    struct block * second;
    parent ->header.size = first_size;
    parent -> header.size |= busy; // marking parent as busy
    if (second_size > 0) {
        second = (struct block *) ((void *) parent + first_size + HEADER_OFFSET);
        second->header.size = second_size;
        second->header.previous_size = first_size;
    } else {
        second = parent;
    }
    struct block * following;
    if (has_next_block(second))
    {
        following = (struct block*)((void*)second + second->header.size + HEADER_OFFSET);
        following->header.previous_size = second_size;
    }
    return parent;
}

struct block * seek_free(struct block * ptr, size_t size);

// memory allocation with 4-byte aligment
void * mem_alloc(size_t size)
{
    struct block * busy_block;
    struct block * free_block;
    struct block * prev_block;
    size_t tmp;
    size_t temp_size;
    size_t prev_block_size;
    size_t data_ptr;
    free_block = seek_free(last_free, size); // thus we have a pointer to a free block
 //   cout << "inside mem_alloc " << free_block << endl;
    if (free_block == NULL)
        return NULL;

    data_ptr = (size_t) (&free_block->header + HEADER_OFFSET);
    tmp = data_ptr & 0x3;
    cout << tmp << endl;
    if (tmp != 0) // aligning to 4-byte retrieve style
    {
        prev_block = (struct block*)((void*)free_block - free_block->header.previous_size - HEADER_OFFSET);
        prev_block->header.size += (0x3 - tmp+1);
        temp_size = free_block->header.size - 0x3 + tmp;
        prev_block_size = free_block->header.previous_size + 0x3 - tmp+1;
        free_block = (struct block*)((void*)free_block - 0x3 + tmp+1); // reallocating block  !!!!!!!!
        free_block->header.previous_size = prev_block_size;
        free_block->header.size = temp_size;
        cout << "bad aligment " << tmp << endl;
    }

    busy_block = divide_block(free_block, size);

    return (&busy_block->header) + HEADER_OFFSET; // taking into account align
}

struct block * seek_free(struct block * ptr, size_t size)
{
    bool cyc_busy;
    size_t tmp;

    while ((cyc_busy = (bool)((ptr->header.size & busy) >> (sizeof(size_t) -1))) ||         // it's a kind of magic
           ((ptr->header.size) < (size + (size_t)(&ptr->header+ HEADER_OFFSET) & 0x3 ^ 0x3 +1 & 0x3 )))        // seeking for a free block with an
    {                                                                       // appropriate size. taking into account
        tmp = ptr->header.size;                                             // memory alignment when choosing a block
  //      cout << "ptr-> header.size" << tmp << endl;
        if (cyc_busy)
            tmp ^= busy; // excretion of of real size

        tmp += HEADER_OFFSET;
  //      cout << ptr << endl;
        if (((void*)ptr+tmp) >= (head + mem_size - HEADER_OFFSET - ((size_t)(&ptr->header + HEADER_OFFSET) & 0x3))) // also subtract 3 in case of bad memory align
            return NULL; // if current available resources are unable to create one more block with header, return null.
        ptr = (struct block*)((void*)ptr + tmp);
    }
    return ptr;
}

bool has_next_block(struct block * ptr);

struct block * merge_blocks(struct block * left, struct block * right)
{
    struct block * response = left;
    struct block * temp;
    size_t lenght_add = 0;
    lenght_add += right->header.size + HEADER_OFFSET;
    response->header.size += lenght_add;
    if (has_next_block(right))
    {
        temp = right + lenght_add;
        temp->header.previous_size = response->header.size;
    }
    return response;
}

bool is_free(struct block * block) {
    return (bool) ((block->header.size & busy) >> (sizeof(size_t)-1));
}

void mem_free(void * addr)
{
    struct block * busy_block;



    busy_block = (struct block*)(addr - HEADER_OFFSET);

}
