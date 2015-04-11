#ifndef __L_LOGICSYS_H__
#define __L_LOGICSYS_H__

#ifdef ASYNCH
void *L_logicinit(void *);
#else
void L_logicinit();
#endif

void L_logicclose();
void L_loadconsole();
void L_enableconsole();
void L_disableconsole();
void L_update();

#endif

