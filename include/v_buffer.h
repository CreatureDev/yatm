#ifndef __V_BUFFER_H__
#define __V_BUFFER_H__

typedef struct model_s model_t;

void V_bufferinit();
void V_bufferclose();

void V_storebuffermodel(const model_t, void *, void *);
void V_removebuffermodel(const model_t);

void V_bindverts(const model_t);
//int V_allocuniform(unsigned long size);
void V_setuniform(int uoff, unsigned long size, void *dat);
void V_freeuniform(int uoff);

#endif

