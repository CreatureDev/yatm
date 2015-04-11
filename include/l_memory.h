#ifndef __L_MEMORY_H__
#define __L_MEMORY_H__

void L_memoryinit();
void L_memoryclose();
void *L_alloc(unsigned long);
void L_free(void *);

#endif

