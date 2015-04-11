#ifndef __V_MEMORY_H__
#define __V_MEMORY_H__

void V_memoryinit();
void V_memoryclose();
void *V_alloc(unsigned long);
void V_free(void *);

#endif

