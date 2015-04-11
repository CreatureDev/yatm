#ifndef __M_MESSAGE_H__
#define __M_MESSAGE_H__

typedef struct msg_s msg_t;

#define ENT_CNT 0x8000

#define TARGET_MSK (~(~0UL << 5))
#define DIRECT_MSK (1UL << 5)
#define COMMAND_MSK (~(~0UL << 10) << 6)
#define DATA_MSK (~(~0UL << 16) << 16)
#define ENT_MSK (~0UL << 32)

#define SET_TARGET(x, t) x = ((x & ~TARGET_MSK) | (t &  TARGET_MSK))
#define MSG_TARGET(x) (x & TARGET_MSK)

#define SET_DIRECT(x, t) x = ((x & ~DIRECT_MSK) | ((t << 3) & DIRECT_MSK))
#define MSG_DIRECT(x) ((x & DIRECT_MSK) >> 3)

#define SET_COMMAND(x, t) x = ((x & ~COMMAND_MSK) | ((t << 6) & COMMAND_MSK))
#define MSG_COMMAND(x) ((x & COMMAND_MSK) >> 6)

#define SET_DATA(x, t) x = ((x & ~DATA_MSK) | ((t << 16) & DATA_MSK))
#define MSG_DATA(x) ((x & DATA_MSK) >> 16)

#define SET_ENT(x, t) x = ((x & ~ENT_MSK) | (t << 32))
#define MSG_ENT(x) ((x & ENT_MSK) >> 32)

#define MSG_RECIEVE 0 /* being passed data */
#define MSG_TRANSMIT 1 /* request for data */

#define TARGET_MAIN (1 << 0)
#define TARGET_VIS (1 << 1)
#define TARGET_PHY (1 << 2)
#define TARGET_LOG (1 << 3)
#define TARGET_AUD (1 << 4)

struct msg_s
{
	unsigned long prim;
	union
	{
		unsigned long dat;
		void *ptr;
	} scnd;
};

void M_sendmessage(const msg_t);

enum msgcmds_e
{
	NULLCMD = 0,
	CREATE_ENT,
	DELETE_ENT,
	CLEAR_ENT,
	TEXTURE,
	MODEL,
	MODEL_MODIFY,
	ENABLE,
	DISABLE,
	INPUT
	
};

enum msgptype_e
{
	NULLTYPE = 0,
	CHAR,
	INT,
	UINT,
	LONG,
	ULONG,
	FLOAT,
	DOUBLE
};

#endif /* m_message.h */

