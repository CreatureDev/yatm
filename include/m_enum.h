#ifndef __M_ENUM_H__
#define __M_ENUM_H__


enum Menum_e
{
	M_FALSE = 0,
	M_TRUE = 1,

#ifdef ASYNCH
	M_WAIT,
	M_CLOSE,
	M_CONTINUE,
#endif

	V_STRT = 0x00100000,
	
	P_STRT = 0x00200000,
	
	L_STRT = 0x00300000,
	
	A_STRT = 0x00400000,

#ifdef ASYNCH	
	V_CONT = 0x01000000,
	P_CONT = 0x02000000,
	L_CONT = 0x04000000,
	A_CONT = 0x08000000,
	V_WAIT = 0x10000000,
	P_WAIT = 0x20000000,
	L_WAIT = 0x40000000,
	A_WAIT = 0x80000000,
#endif

	M_BUNK = 0xffffffff
};

typedef enum Menum_e Menum;

#endif

