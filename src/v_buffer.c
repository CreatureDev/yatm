#include "../include/v_buffer.h"
#include "../include/v_memory.h"
#include "../include/v_types.h"

#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>

#define BBUF 0x1
#define FBUF 0x2
#define UBUF 0x4

#define V_SIZE(x) (sizeof(float) * 3 * x)
#define T_SIZE(x) (sizeof(float) * 2 * x)
#define N_SIZE(x) (sizeof(float) * 3 * x)
#define F_SIZE(x) (sizeof(unsigned int) * 3 * x)

#define ALLOC_BIT 0x80000000

#define BBUF_SIZE 0x100000
#define BBLK_SIZE (sizeof(float) * 16)
#define UBUF_SIZE 0x20000
#define UBLK_SIZE 0x20
#define FBUF_SIZE 0x60000
#define FBLK_SIZE (sizeof(unsigned int) * 8)

/* This file manages buffer allocation for models
 * and buffer data
 * It uses an allocator that is likely to be very slow
 * but uses as little data as possible
 * Feel free to redo it if necessary, which may be likely
*/

typedef struct bblock_s bblock_t;

struct bblock_s
{
	unsigned long size;
};

static struct
{
	unsigned int name;
} base;

static struct
{
	unsigned int name;
} face;

static struct
{
	unsigned int name;
} unif;

static void *bres;
static void *ures;
static void *fres;

void glGenBuffers(GLsizei, GLuint *);
void glBindBuffer(GLenum, GLuint);
void glBufferSubData(GLenum, GLintptr, GLsizeiptr, const GLvoid *);
void glBufferData(GLenum, GLsizeiptr, const GLvoid *, GLenum);
void glVertexAttribPointer(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);

static void V_genbuffer(unsigned int *buf)
{
	glGenBuffers(1, buf);
}

static void V_bindbuffer(unsigned long type, unsigned int buff)
{
	switch(type)
	{
	default:
		//nothing to see here, just preparing for the future
		type = buff = 7;
	}
	glBindBuffer(GL_ARRAY_BUFFER, base.name);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, face.name);
	glBindBuffer(GL_UNIFORM_BUFFER, unif.name);
}

void V_bindverts(const model_t mdl)
{
	V_bindbuffer(7, 7);
	glVertexAttribPointer(VPOS_INDEX, 3, GL_FLOAT, GL_FALSE, 0, *mdl.vrt);
	glVertexAttribPointer(VTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, *mdl.tex);
	glVertexAttribPointer(VNRM_INDEX, 3, GL_FLOAT, GL_FALSE, 0, *mdl.nrm);
}

static void *V_allocbuffer(unsigned long size, unsigned int btype)
{
	bblock_t *blk;
	unsigned long blks, len, i, nblk;
	switch(btype)
	{
	case BBUF:
		blk = (bblock_t *) bres;
		blks = BBLK_SIZE;
		nblk = BBUF_SIZE / BBLK_SIZE;
		break;
	case UBUF:
		blk = (bblock_t *) ures;
		blks = UBLK_SIZE;
		nblk = UBUF_SIZE / UBLK_SIZE;
		break;
	case FBUF:
		blk = (bblock_t *) fres;
		blks = FBLK_SIZE;
		nblk = FBUF_SIZE / FBLK_SIZE;
		break;
	default:
		return 0;
	}
	len = size / blks;
	if(size % blks)
		len++;
	i = 1;
	while(i < nblk)
	{
		if(blk[i].size & ALLOC_BIT)
		{
			i += blk[i].size ^ ALLOC_BIT;
			continue;
		}
		if(len < blk[i].size)
		{
			blk[i+len].size = blk[i].size - len;
			blk[i].size |= ALLOC_BIT;
			return (void *) (i * blks);
		}
		if(len == blk[i].size)
		{
			blk[i].size |= ALLOC_BIT;
			return (void *) (i * blks);
		}
		else
		{
			i += blk[i].size;
		}
	}
	return 0;
}

static void V_freebuffer(unsigned long off, unsigned int btype)
{
	bblock_t *blk;
	unsigned long nblk, id, i, j;
	if(!off)
		return;
	switch(btype)
	{
	case BBUF:
		id = off / BBLK_SIZE;
		nblk = BBUF_SIZE / BBLK_SIZE;
		blk = (bblock_t *) bres;
		break;
	case UBUF:
		id = off / UBLK_SIZE;
		nblk = UBUF_SIZE / UBLK_SIZE;
		blk = (bblock_t *) ures;
		break;
	case FBUF:
		id = off / FBLK_SIZE;
		nblk = FBUF_SIZE / FBLK_SIZE;
		blk = (bblock_t *) fres;
		break;
	default:
		return;
	}
	blk[id].size ^= ALLOC_BIT;
	i = 1;
	while(i < id)
	{
		if(blk[i].size & ALLOC_BIT)
		{
			i += blk[i].size ^ ALLOC_BIT;
			continue;
		}
		if(blk[i + blk[i].size].size & ALLOC_BIT)
		{
			i += blk[i].size + blk[i + blk[i].size].size;
			continue;
		}
		j = blk[i + blk[i].size].size;
		blk[i + blk[i].size].size = 0;
		blk[i].size += j;
		if(i + blk[i].size > id)
		{
			id = i;
		}
	}
	if(id + blk[id].size < nblk)
	{
		if(blk[id + blk[id].size].size & ALLOC_BIT)
			return;
		j = blk[id + blk[id].size].size;
		blk[id + blk[id].size].size = 0;
		blk[id].size += j;
	}
}

void V_setuniform(int uoff, unsigned long size, void *dat)
{
	glBufferSubData(GL_UNIFORM_BUFFER, uoff, size, dat);
}

/* void *V_allocuniform(unsigned long size)
 * {
 *	return V_allocbuffer(size, UBUF);
 * }
*/

void V_freeuniform(int uoff)
{
	V_freebuffer(uoff, UBUF);
}

void V_storebuffermodel(const model_t mod, void *vdat, void *fdat)
{
	void *tdat, *ndat;
	*mod.vrt = V_allocbuffer(V_SIZE(*mod.vcnt), BBUF);
	tdat = vdat + V_SIZE(*mod.vcnt);
	*mod.tex = V_allocbuffer(T_SIZE(*mod.vcnt), BBUF);
	ndat = tdat + T_SIZE(*mod.vcnt);
	*mod.nrm = V_allocbuffer(N_SIZE(*mod.vcnt), BBUF);
	*mod.fac = V_allocbuffer(F_SIZE(*mod.fcnt), FBUF);
	if(!(*mod.vrt && *mod.tex && *mod.nrm && *mod.fac))
	{
		V_freebuffer((unsigned long) *mod.vrt, BBUF);
		V_freebuffer((unsigned long) *mod.tex, BBUF);
		V_freebuffer((unsigned long) *mod.nrm, BBUF);
		V_freebuffer((unsigned long) *mod.fac, FBUF);
		*mod.vrt = *mod.tex = *mod.nrm = *mod.fac = 0;
		return;
	}

	glBufferSubData(GL_ARRAY_BUFFER, (GLintptr) *mod.vrt, V_SIZE(*mod.vcnt), vdat);
	glBufferSubData(GL_ARRAY_BUFFER, (GLintptr) *mod.tex, T_SIZE(*mod.vcnt), tdat);
	glBufferSubData(GL_ARRAY_BUFFER, (GLintptr) *mod.nrm, N_SIZE(*mod.vcnt), ndat);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (GLintptr) *mod.fac, F_SIZE(*mod.fcnt), fdat);
}

void V_removebuffermodel(const model_t mod)
{
	V_freebuffer((unsigned long) *mod.vrt, BBUF);
	V_freebuffer((unsigned long) *mod.tex, BBUF);
	V_freebuffer((unsigned long) *mod.nrm, BBUF);
	V_freebuffer((unsigned long) *mod.fac, FBUF);
	*mod.vrt = *mod.tex = *mod.nrm = *mod.fac = 0;
	*mod.vcnt = *mod.fcnt = 0;
}

void V_bufferinit()
{
	bblock_t *blks;
	unsigned long i;
	if(bres || ures || fres)
		return;
	bres = V_alloc((BBUF_SIZE/BBLK_SIZE) * sizeof(bblock_t));
	ures = V_alloc((UBUF_SIZE/UBLK_SIZE) * sizeof(bblock_t));
	fres = V_alloc((FBUF_SIZE/FBLK_SIZE) * sizeof(bblock_t));
	if(!(bres && ures && fres))
	{
		printf("failed to allocate buffer manager\n");
		exit(-1);
	}
	V_genbuffer(&(base.name));
	V_genbuffer(&(face.name));
	V_genbuffer(&(unif.name));
	
	V_bindbuffer(11, 14);
	
	glBufferData(GL_ARRAY_BUFFER, BBUF_SIZE, '\0', GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, FBUF_SIZE, '\0', GL_DYNAMIC_DRAW);
	glBufferData(GL_UNIFORM_BUFFER, UBUF_SIZE, '\0', GL_DYNAMIC_DRAW);
	
	blks = (bblock_t *) bres;
	blks[1].size = BBUF_SIZE / BBLK_SIZE - 1;
	for(i = 2; i < (BBUF_SIZE / BBLK_SIZE); i++)
		blks[i].size = 0;

	blks = (bblock_t *) ures;
	blks[1].size = UBUF_SIZE / UBLK_SIZE - 1;
	for(i = 2; i < (UBUF_SIZE / UBLK_SIZE); i++)
		blks[i].size = 0;
	
	blks = (bblock_t *) fres;
	blks[1].size = FBUF_SIZE / FBLK_SIZE - 1;
	for(i = 2; i < (FBUF_SIZE / FBLK_SIZE); i++)
		blks[i].size = 0;
}

void V_bufferclose()
{
	if(bres)
		V_free(bres);
	if(ures)
		V_free(ures);
	if(fres)
		V_free(fres);
	bres = ures = fres = '\0';
}



