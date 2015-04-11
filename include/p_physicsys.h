#ifndef __P_PHYSICSYS_H__
#define __P_PHYSICSYS_H__

#ifdef ASYNCH
void *P_physicinit(void *);
#else
void P_physicinit();
#endif

void P_physicclose();

#endif

