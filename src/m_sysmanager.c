#ifndef SYSMAN
#define SYSMAN
#endif

#include "../include/m_state.h"
#include "../include/m_sysmanager.h"
#include "../include/m_memory.h"
#include "../include/m_message.h"
#include "../include/v_visualsys.h"
#include "../include/p_physicsys.h"
#include "../include/a_audiosys.h"
#include "../include/l_logicsys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char M_basedir[BASEDIR_BUFF];
static unsigned int ecnt;
unsigned short active[ENT_REF_SIZE];

unsigned int m_state = 0;

static void M_parsearguments()
{
	extern int m_argc;
	extern char **m_argv;
	if(m_argc == 1)
		return;
	m_argv[1][0] = '\0';
}

void V_processmessage(const msg_t);
void P_processmessage(const msg_t);
void L_processmessage(const msg_t);
void A_processmessage(const msg_t);

static void M_processmessage(const msg_t mes)
{
	
}

void M_sendmessage(const msg_t mes)
{
	unsigned int trg;
	trg = MSG_TARGET(mes.prim);
	if(trg & TARGET_MAIN)
		M_processmessage(mes);
	if(trg & TARGET_VIS)
		V_processmessage(mes);
	if(trg & TARGET_PHY)
		P_processmessage(mes);
	if(trg & TARGET_LOG)
		L_processmessage(mes);
	if(trg & TARGET_AUD)
		A_processmessage(mes);
}

void M_systemsinit()
{
	int i;
	char *ydir;
	FILE *pwd = popen("pwd", "r");
	if(!pwd || !fgets(M_basedir, BASEDIR_BUFF, pwd))
	{
		printf("command pwd failed\n");
		exit(-1);
	}
	i = strlen(M_basedir);
	if(!i)
	{
		printf("pwd returned string of length 0\n");
		exit(-1);
	}
	ydir = strstr(M_basedir, "yatm");
	if(ydir)
	{
		ydir[4] = '\0';
	}
	else
	{
		printf("game resource directory location is unknown\n");
		exit(-1);
	}
	M_memoryinit();
	V_visualinit();
	P_physicinit();
	L_logicinit();
	A_audioinit();
	for(ecnt = 0; ecnt < ENT_REF_SIZE; ecnt++)
	{
		active[ecnt] = ecnt;
	}
	ecnt = 0;
	M_parsearguments();
}

void M_systemsclose()
{
	A_audioclose();
	L_logicclose();
	P_physicclose();
	V_visualclose();
	M_memoryclose();
}

