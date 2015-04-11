#ifndef __A_AUDIOSYS_H__
#define __A_AUDIOSYS_H__

#ifdef ASYNCH
void *A_audioinit(void *);
#else
void A_audioinit();
#endif

void A_audioclose();

#endif

