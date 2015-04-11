#ifndef __V_PROGRAM_H__
#define __V_PROGRAM_H__

unsigned int V_getprogram(const char *vsh, const char *fsh);
unsigned int V_getuniformlocation(unsigned int, const char *);
unsigned int V_getattriblocation(unsigned int, const char *);
void V_defaultattribloc(unsigned int);
void V_removeprogram(unsigned int);
void V_useprogram(unsigned int);
void V_programinit();
void V_programclose();

#endif

