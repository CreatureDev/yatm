#ifndef __OBJVERTLIST_H__
#define __OBJVERTLIST_H__

#define NULLID 0xffffffff

void vertlistinit(unsigned int);
void freevertlist();
unsigned int *getvert(unsigned int);
unsigned int getvertid(char *);
unsigned int numunique();
void addvert(char *);
void idverts();

#endif

