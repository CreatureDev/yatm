#ifndef __M_MEMORY_H__
#define __M_MEMORY_H__

void *M_alloc(unsigned long);
void M_free(void *);
void M_memoryinit();
void M_memoryclose();

#endif

