#include <iostream>
#include "Allocator.h"


using namespace std;

int main() {
    int * i, *a, *b ,*c, *d;
    long * l, *z, *x, *n, *m;
    void * addr1, *addr2, *addr3, *addr4, *addr5;

    cout << "Hello, World!" << endl;
    init(malloc(2000), 2000);
    cout << "sizeof(struct block) " << sizeof(struct block) << endl;
    cout << "size_t " << sizeof(size_t) << endl;
    cout << "sizeof(header_block) " << sizeof(block_header) << endl;
    cout << "szieof(data_block)" << sizeof(data) << endl;
    cout << "sizeof(void*)" << sizeof(void*) << endl;
    cout << sizeof(unsigned char) << endl;
    cout << last_free << endl;
    cout << (i = (int*)mem_alloc(16)) << endl;
    mem_free(i);
    cout << (addr1 = mem_alloc(20)) << endl;
    mem_dump(i);
    cout << (addr2 = mem_alloc(50)) << endl;
    mem_free(addr1);
    cout << (a = (int*)mem_alloc(16)) << endl;
    mem_dump(i);
    cout << mem_alloc(7) << endl;
    mem_dump(i);
    cout << (a = (int*)mem_realloc(a, sizeof(int))) << endl;
    mem_dump(i);
    cout << (l = (long*)mem_alloc(sizeof(long))) << endl;
    mem_dump(i);
    cout << mem_alloc(20) << endl;
    mem_free(a);
    mem_dump(i);
    cout << mem_alloc(30) << endl;
    mem_dump(i);
    cout << (l = (long*)mem_realloc(l, (2*sizeof(long)))) << endl;
    mem_dump(i);
    cout << mem_alloc(40) << endl;
    mem_dump(i);
    cout << (addr3 = mem_alloc(13)) << endl;
    mem_dump(i);
    cout << mem_alloc(8) << endl;
    mem_dump(i);
    cout << mem_alloc(50) << endl;
    mem_dump(i);
    mem_free(addr3);
    cout << mem_alloc(50) << endl;
    mem_dump(i);
    return 0;
}