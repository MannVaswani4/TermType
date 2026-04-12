#ifndef MEMORY_H
#define MEMORY_H

/* Simple bump allocator backed by a global virtual RAM pool */
void *my_alloc(int size);

/* Stack-discipline dealloc: pass the pointer returned by my_alloc to free it
   and everything allocated after it (resets the pool offset to that pointer). */
void  my_dealloc(void *ptr);

/* Reset the entire pool (useful at program end) */
void  my_reset(void);

#endif