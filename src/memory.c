#include "memory.h"

/* A fixed pool of bytes; bump a pointer forward on each allocation */
#define POOL_SIZE 65536

static char  pool[POOL_SIZE];
static int   pool_offset = 0;

void *my_alloc(int size) {
    if (pool_offset + size > POOL_SIZE) {
        return 0;  /* out of memory — return NULL equivalent */
    }
    void *ptr = &pool[pool_offset];
    pool_offset += size;
    return ptr;
}