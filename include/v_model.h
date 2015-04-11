#ifndef __V_MODEL_H__
#define __V_MODEL_H__

unsigned int V_getmodel(const char *);
void V_bindmodel(unsigned int);
void V_removemodel(unsigned int);
void V_modelinit();
void V_modelclose();

#endif

