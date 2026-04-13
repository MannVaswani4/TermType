#ifndef MEMORY_H
#define MEMORY_H

/* Allocate 'size' bytes from a fixed memory pool */
void *my_alloc(int size);

/* Reset entire memory pool (useful between runs if needed) */
void my_reset(void);

#endif