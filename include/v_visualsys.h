#ifndef __V_VISUALSYS_H__
#define __V_VISUALSYS_H__

#ifdef ASYNCH
void *V_visualinit(void *);
#else
void V_visualinit();
#endif

void V_loadconsole();
void V_visualclose();
void V_render();
void V_update();

#endif

