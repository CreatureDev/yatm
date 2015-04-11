#ifndef __M_SYSMANAGER_H__
#define __M_SYSMANAGER_H__

#ifdef ASYNCH
void M_continue();
void M_wait();
#endif

#ifdef SYSMAN
#define BASEDIR_BUFF 256
#define ENT_REF_SIZE 0x10000
#endif

void M_systemsinit();
void M_systemsclose();

#endif

