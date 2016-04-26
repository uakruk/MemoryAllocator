#include <iostream>
#include "Allocator.h"


using namespace std;

int main() {
    int * i;
    cout << "Hello, World!" << endl;
    init(malloc(1000), 900);
    cout << "sizeof(struct block) " << sizeof(struct block) << endl;
    cout << "size_t " << sizeof(size_t) << endl;
    cout << "sizeof(header_block) " << sizeof(block_header) << endl;
    cout << "szieof(data_block)" << sizeof(data) << endl;
    cout << "sizeof(void*)" << sizeof(void*) << endl;
    cout << last_free << endl;
    cout << (i = (int*)mem_alloc(16)) << endl;
    mem_free(i);
    cout << mem_alloc(20) << endl;
    cout << mem_alloc(16) << endl;
    cout << mem_alloc(1) << endl;
    cout << mem_alloc(2) << endl;
    cout << mem_alloc(10) << endl;
    cout << mem_alloc(20) << endl;
    cout << mem_alloc(30) << endl;
    cout << mem_alloc(40) << endl;
    cout << mem_alloc(7) << endl;
    cout << mem_alloc(50) << endl;
    mem_dump(i);
    return 0;
}