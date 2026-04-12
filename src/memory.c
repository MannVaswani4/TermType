#include "memory.h"

/* A fixed pool of bytes simulating virtual RAM */
#define POOL_SIZE 65536

static char pool[POOL_SIZE];
static int  pool_offset = 0;

void *my_alloc(int size) {
    if (pool_offset + size > POOL_SIZE) {
        return 0;  /* out of memory */
    }
    void *ptr = &pool[pool_offset];
    pool_offset += size;
    return ptr;
}

void my_dealloc(void *ptr) {
    /* Stack-discipline: reset offset back to where ptr starts in the pool */
    char *p = (char *)ptr;
    if (p >= pool && p < pool + POOL_SIZE) {
        pool_offset = (int)(p - pool);
    }
}

void my_reset(void) {
    pool_offset = 0;
}