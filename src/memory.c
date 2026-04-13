#include "memory.h"

/* Simulated virtual RAM */
#define POOL_SIZE 65536

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

/* Reset entire pool */
void my_reset(void) {
    offset = 0;
}