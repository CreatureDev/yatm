#include "../include/l_entity.h"
#include "../include/m_message.h"
#include "../include/l_memory.h"

static unsigned long *dsc = '\0';

void L_processmessage(const msg_t mes)
{
		
}

void L_entityinit()
{
	if(dsc)
		return;
	dsc = (unsigned long *) L_alloc(sizeof(unsigned long) * ENT_CNT);
}

void L_entityclose()
{
	if(dsc)
		L_free(dsc);
	dsc = '\0';
}

