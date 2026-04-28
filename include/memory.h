#ifndef MEMORY_H
#define MEMORY_H

/* Allocate 'size' bytes from a fixed memory pool */
void *my_alloc(int size);


/* Bump-dealloc: resets pool offset back to ptr (LIFO only) */
void my_dealloc(void *ptr);

#endif