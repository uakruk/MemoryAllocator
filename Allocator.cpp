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
    if ((void*)ptr + (is_free(ptr) ? ptr->header.size : ptr->header.size ^ busy) + HEADER_OFFSET >= (head + mem_size))
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
 //       cout << second_size << endl;
  //      cout << first_size << endl;
    } else {
        second = parent;
 //       cout << "else" << endl;
    }
    struct block * following;
    if (has_next_block(second))
    {
        cout << second << endl;
        following = (struct block*)((void*)second + second->header.size + HEADER_OFFSET);
        following->header.previous_size = second_size;
//        cout << "tut" << endl;
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
    free_block = seek_free((struct block*)head, size); // thus we have a pointer to a free block
    //   cout << "inside mem_alloc " << free_block << endl;
    if (free_block == NULL)
        return NULL;

    data_ptr = (size_t) (&free_block->header + HEADER_OFFSET);
    tmp = data_ptr & 0x3;
    //   cout << tmp << endl;
    if (tmp != 0) // aligning to 4-byte retrieve style
    {
        prev_block = (struct block*)((void*)free_block - free_block->header.previous_size - HEADER_OFFSET);
        size_t val = (0x3 - tmp + 1);
        prev_block->header.size += val;
        temp_size = free_block->header.size - val;
        prev_block_size = free_block->header.previous_size + val;
        free_block = (struct block*)((void*)free_block + val); // reallocating block  !!!!!!!!
        free_block->header.previous_size = prev_block_size;
        free_block->header.size = temp_size;
//        cout << "bad aligment " << tmp << endl;
    }

    busy_block = divide_block(free_block, size);

    return (void*)busy_block + HEADER_OFFSET; // taking into account align
}

struct block * seek_free(struct block * ptr, size_t size)
{
    bool cyc_busy;
    size_t tmp;

    while ((cyc_busy = (bool)((ptr->header.size & busy) >> (8*sizeof(size_t) -1))) ||         // it's a kind of magic
           ((ptr->header.size) < (size + ((size_t)(&ptr->header+ HEADER_OFFSET) & 0x3 ^ 0x3 +1 & 0x3 ))))        // seeking for a free block with an
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

// assuming block merge can be done only with free blocks
struct block * merge_blocks(struct block * left, struct block * right)
{
//    cout << "inside merge " << left->header.previous_size << endl;
//    cout << "inside merge " << left->header.size << endl;
//    cout << "inside merge " << right->header.size << endl;
    struct block * response = left;
    struct block * temp;
    size_t lenght_add = right->header.size;

    lenght_add += HEADER_OFFSET;
    response->header.size += lenght_add;
    if (has_next_block(right))
    {
        temp = (struct block*)((void*)right + lenght_add);
        temp->header.previous_size = response->header.size;
    }
    return response;
}

bool is_free(struct block * block) {
    return (bool) !((block->header.size & busy) >> (sizeof(size_t)-1));
}

struct block * next_block(struct block * current) {
//    cout << "this " << current->header.size << endl;
//    cout << is_free(current) << endl;
    struct block * response = (struct block*)((void*)current + HEADER_OFFSET + (is_free(current) ?
                                                                                current->header.size :
                                                                                (current->header.size ^ busy)));
    //   cout << response << endl;
    return response;
}

struct block * prev_block(struct block * current) {
    return (struct block*)((void*)current - current->header.previous_size - HEADER_OFFSET);
}

void mem_free(void * addr)
{
    struct block * busy_block;
    struct block * check_block;
    struct block * work_block;

    busy_block = (struct block*)(addr - HEADER_OFFSET);
    busy_block->header.size ^= busy;  //before merging, left block must be free
    work_block = busy_block;
    //   cout << "inside mem_free" << endl;
//    cout << work_block->header.previous_size << endl;
    if (has_prev_block(work_block) && is_free(check_block = prev_block(work_block)))
        work_block = merge_blocks(check_block, work_block);
    //   cout << "merge doesn't work" << endl;
    //   cout << is_free(next_block(work_block)) << "sadf" << endl;
    if (has_next_block(work_block) && is_free(check_block = next_block(work_block)))
        work_block = merge_blocks(work_block, check_block);
//    cout << last_free << endl;
    if ((void*)work_block < (void*)last_free)
        last_free = work_block;
//    cout << last_free << endl;
}

void mem_dump(void * start) {
    struct block * ptr = (struct block*)(start - HEADER_OFFSET);
    struct block * curr;
    int i = 1;
    //   cout << (ptr->header.previous_size) << endl;
    do {
        curr = ptr;
        cout << "Block " << i++ << " is " << (is_free(ptr) ? "free " : "busy ") << "and has size of "
        << (is_free(ptr) ? ptr->header.size : ptr->header.size ^ busy) << endl;
        ptr = (struct block*)((void*)curr + HEADER_OFFSET + (is_free(curr) ?
                                                             ptr->header.size :
                                                             ptr->header.size ^ busy));
    } while (has_next_block(curr));
}

void mem_copy(void *addr1, void *addr2, size_t size) {
    char data;
    size_t iter = 0;
    while (iter < size) {
        data = *(char*)(addr1 + iter);
        *(char*)(addr1 + iter) = *(char*)(addr2 + iter);
        *(char*)(addr2 + iter) = data;
        iter++;
    }
}

void mem_recovery(bool left, struct block * b_left, bool right, struct block * b_right, struct block * middle, size_t middle_size) {
    struct block * rbl;
    struct block * lbl;
    if (left) {
        lbl = (struct block*)((void*)middle - b_left->header.size - HEADER_OFFSET);
        *lbl = *b_left;
    }
    if (right) {
        rbl = (struct block*)((void*)middle + middle_size + HEADER_OFFSET);
        *rbl = *b_right;
    }
}

void * mem_realloc(void * addr, size_t size) {
    struct block * temp_block;
    struct block rem_block;
    struct block mem_dump_left;
    struct block mem_dump_right;
    bool left_rescue = false;
    bool right_rescue = false;
    void * ptr;
    if (addr == NULL)
        return mem_alloc(size);
    struct block * curr = (struct block*)(addr - HEADER_OFFSET);
    struct block curr_dump = *curr;
    rem_block = *curr;
    ptr = curr;
    size_t curr_size = curr->header.size ^ busy; // there block is busy
    if (size > curr_size) {
        if (has_prev_block(curr) && is_free(prev_block(curr))) {
            mem_dump_left = *prev_block(curr);
            left_rescue = true;
        }
        if (has_next_block(curr) && is_free((next_block(curr)))) {
            mem_dump_right = *next_block(curr);
            right_rescue = true;
        }
        mem_free(addr);
        ptr = mem_alloc(size);
        if (ptr == NULL) {
            mem_recovery(left_rescue, &mem_dump_left, right_rescue, &mem_dump_right, curr, curr_size);
            *curr = curr_dump;
            return NULL;
        } else {
            mem_copy((void*)curr + HEADER_OFFSET, ptr, curr_size);
        }
    } else {
        mem_free(addr);
        ptr = mem_alloc(size);
        mem_copy((void*)curr+ HEADER_OFFSET, ptr, size);
    }
    return ptr;
}
