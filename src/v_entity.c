#include "../include/v_entity.h"
#include "../include/v_memory.h"
#include "../include/v_model.h"
#include "../include/v_program.h"
#include "../include/m_message.h"

#include <stdio.h>
#include <GL/gl.h>

void glGenVertexArrays(GLsizei, GLuint *);
void glDeleteVertexArrays(GLsizei, GLuint *);

#define DEF_RIGID_V "chain.vv"
#define DEF_RIGID_F "chain.vf"

#define VISMASK 1
#define TYPEMASK (~(~0 << 3) << 1)
#define VENTTYPE(x) ((x & TYPEMASK) >> 1)
#define SETVENTTYPE(x, t) x = (x & ~TYPEMASK) | ((t << 1) & TYPEMASK)
#define RIGIDVENT 1
#define SKELEVENT 2
#define CLOTHVENT 3
#define FLUIDVENT 4
#define PARTIVENT 5
#define GUIVENT 6

static unsigned long *dsc = '\0';
static unsigned int *vao = '\0';

//f1 and f2 are generic visual entity data fields
//their contents and usage is defined by dsc
static unsigned int *f1 = '\0';
static unsigned int *f2 = '\0';

static void V_clearent(const msg_t mes)
{
	unsigned int ent;
	ent = MSG_ENT(mes.prim);
	if(MSG_DIRECT(mes.prim) == MSG_RECIEVE)
	{
//		V_unusemodel(f1[ent]);
//		or GUI or particles or whatever
//		V_unusetexture(f2[ent]);
		dsc[ent] = 0;
		f1[ent] = f2[ent] = 0;
		return;
	}
	//return long value or string description ??
	//probably
	//maybe
	dsc[ent] = 0;
}

static void V_texture(const msg_t mes)
{

}

static void V_model(const msg_t mes)
{

}

static void V_enable(const msg_t mes)
{

}

static void V_disable(const msg_t mes)
{

}

void V_processmessage(const msg_t mes)
{
	switch(MSG_COMMAND(mes.prim))
	{
	case CLEAR_ENT:
		V_clearent(mes);
		break;
	case TEXTURE:
		V_texture(mes);
		break;
	case MODEL:
		V_model(mes);
		break;
	case ENABLE:
		V_enable(mes);
		break;
	case DISABLE:
		V_disable(mes);
		break;
	}
}

void V_entityinit()
{
	unsigned long i;
	if(dsc)
		return;
	i = sizeof(unsigned long) * ENT_CNT;
	dsc = (unsigned long *) V_alloc(i);
	i = sizeof(unsigned int) * ENT_CNT;
	vao = (unsigned int *) V_alloc(i);
	f1 = (unsigned int *) V_alloc(i);
	f2 = (unsigned int *) V_alloc(i);
	glGenVertexArrays(ENT_CNT, vao);
	for(i = 0; i < ENT_CNT; i++)
	{
		dsc[i] = 0;
		f1[i] = f2[i] = 0;
	}
}

void V_entityclose()
{
	if(dsc)
		V_free(dsc);
	if(vao)
	{
		glDeleteVertexArrays(ENT_CNT, vao);
		V_free(vao);
	}
	if(f1)
		V_free(f1);
	if(f2)
		V_free(f2);
	dsc = '\0';
	vao = f1 = f2 = '\0';
}

