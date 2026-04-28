#include "memory.h"

/* Simulated virtual RAM */
#define POOL_SIZE 1048576

static char pool[POOL_SIZE];
static int offset = 0;

/* Allocate memory from pool */
void *my_alloc(int size) {
    if (size <= 0) return 0;

    if (offset + size > POOL_SIZE) {
        return 0;  /* out of memory */
    }

    void *ptr = &pool[offset];
    offset += size;

    return ptr;
}

/* Deallocate by resetting offset back to ptr (LIFO bump dealloc) */
void my_dealloc(void *ptr) {
    char *p = (char *)ptr;
    if (p >= pool && p <= pool + POOL_SIZE) {
        offset = (int)(p - pool);
    }
}