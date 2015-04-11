#include "objvertlist.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct vert_s vert_t;
typedef struct texc_s texc_t;
typedef struct norm_s norm_t;

struct norm_s
{
    unsigned int id;
    unsigned int vid;
    norm_t *next;
};

struct texc_s
{
    unsigned int id;
    texc_t *next;
    norm_t  norm;
};

struct vert_s
{
	texc_t	texc;
};

static vert_t *verts;
static unsigned int vertc; //vertex positions
static unsigned int vcnt; //unique vertecies

unsigned int numunique()
{
	return vcnt;
}

void freenorm(norm_t *ded)
{
	if(ded->next)
		freenorm(ded->next);
	free(ded);
}

void freetexc(texc_t *ded)
{
	if(ded->next)
		freetexc(ded->next);
	if(ded->norm.next)
		freenorm(ded->norm.next);
	free(ded);
}

void freevertlist()
{
	unsigned int i;
	norm_t *tnrm;
	texc_t *ttex;
	if(vertc >= vcnt)
	{
		free(verts);
		return;
	}
	for(i = 0; i < vertc; i++)
	{
		ttex = &(verts[i].texc);
		if(ttex->next)
			freetexc(ttex->next);
		tnrm = &(ttex->norm);
		if(tnrm->next)
			freenorm(tnrm->next);
	}
	free(verts);
}

void vertlistinit(unsigned int vc)
{
	vert_t *tvrt;
	texc_t *ttex;
	norm_t *tnrm;
	if(verts)
		freevertlist();
	vcnt = 0;
	verts = (vert_t *) malloc(sizeof(vert_t) * vc);
	for(vertc = 0; vertc < vc; vertc++)
	{
		tvrt = &verts[vertc];
		ttex = &(tvrt->texc);
		ttex->id = NULLID;
		ttex->next = '\0';
		tnrm = &(ttex->norm);
		tnrm->id = NULLID;
		tnrm->vid = NULLID;
		tnrm->next = '\0';
	}
}

void addvert(char *def)
{
	char *vbuf, *vtbuf, *vnbuf;
	unsigned int tmp;
	vert_t *tmpv;
	texc_t *tmpvt;
	norm_t *tmpvn;
	if(!(strlen(def) && verts))
		return;
	vbuf = def;
	vtbuf = vnbuf = '\0';
	for(tmp = strlen(def); tmp < strlen(def) + 1; tmp--)
	{
		if(def[tmp] == '/')
		{
			if(vnbuf)
			{
				vtbuf = &def[tmp+1];
				break;
			}
			else
				vnbuf = &def[tmp+1];
		}
	}
	tmp = atoi(vbuf) - 1;
	if(!(tmp < vertc))
		return;
	tmpv = &verts[tmp];
	tmp = atoi(vtbuf) - 1;
	tmpvt = &tmpv->texc;
	while(tmp != tmpvt->id)
	{
		if(tmpvt->id == NULLID)
			tmpvt->id = tmp;
		else
		if(tmpvt->next)
			tmpvt = tmpvt->next;
		else
		{
			tmpvt->next = (texc_t *) malloc(sizeof(texc_t));
			tmpvt = tmpvt->next;
			tmpvt->id = tmp;
			tmpvt->norm.id = NULLID;
			tmpvt->norm.vid = NULLID;
			tmpvt->next = '\0';
			tmpvt->norm.next = '\0';
		}
	}
	tmpvn = &tmpvt->norm;
	tmp = atoi(vnbuf) - 1;
	while(tmp != tmpvn->id)
	{
		if(tmpvn->id == NULLID)
			tmpvn->id = tmp;
		else
		if(tmpvn->next)
			tmpvn = tmpvn->next;
		else
		{
			tmpvn->next = (norm_t *) malloc(sizeof(norm_t));
			tmpvn = tmpvn->next;
			tmpvn->id = tmp;
			tmpvn->vid = NULLID;
			tmpvn->next = '\0';
		}
	}

}

void idnorm(norm_t *nrm, unsigned int *cur)
{
	nrm->vid = *cur;
	(*cur)++;
	if(nrm->next)
		idnorm(nrm->next, cur);
}

void idtexc(texc_t *tex, unsigned int *cur)
{
	idnorm(&tex->norm, cur);
	if(tex->next)
		idtexc(tex->next, cur);
}

void idverts()
{
	unsigned int i, cur;
	cur = 0;
	for(i = 0; i < vertc; i++)
		idtexc(&verts[i].texc, &cur);
	vcnt = cur;
}

unsigned int getvertid(char *def)
{
	unsigned int tmp;
	char *vbuf, *vtbuf, *vnbuf;
	vbuf = def;
	vtbuf = vnbuf = '\0';
	for(tmp = 0; tmp < strlen(def); tmp++)
	{
		if(def[tmp] == '/')
		{
			if(vtbuf)
			{
				vnbuf = &(def[tmp+1]);
				break;
			}
			else
				vtbuf = &(def[tmp+1]);
		}
	}
	vert_t tmpv;
	texc_t tmpvt;
	norm_t tmpvn;
	tmp = atoi(vbuf) - 1;
	if(!(tmp < vertc))
		return NULLID;
	tmpv = verts[tmp];
	tmp = atoi(vtbuf) - 1;
	tmpvt = tmpv.texc;
	while(tmp != tmpvt.id)
	{
		if(tmpvt.next)
			tmpvt = *(tmpvt.next);
		else
			return NULLID;
	}
	tmp = atoi(vnbuf) - 1;
	tmpvn = tmpvt.norm;
	while(tmp != tmpvn.id)
	{
		if(tmpvn.next)
			tmpvn = *(tmpvn.next);
		else
			return NULLID;
	}
	return tmpvn.vid;
}

void getnorm(unsigned int *def, norm_t cur, unsigned int pid)
{
	if(pid == cur.vid)
	{
		*def = cur.id;
		return;
	}
	else
	if(cur.next)
		getnorm(def, *(cur.next), pid);
}

void gettexc(unsigned int *def, texc_t cur, unsigned int pid)
{
	getnorm(&def[1], cur.norm, pid);
	if(def[0] != def[1])
	{
		def[0] = cur.id;
		return;
	}
	else
	if(cur.next)
		gettexc(def, *(cur.next), pid);
}

unsigned int *getvert(unsigned int pid)
{
	unsigned int *def, i;
	def = (unsigned int *) malloc(sizeof(unsigned int) * 3);
	if(!(pid < vcnt))
	{
		def[0] = def[1] = def[2] = NULLID;
		return def;
	}
	def[0] = def[1] = def[2] = NULLID - 1;
	for(i = 0; i < vertc; i++)
	{
		gettexc(&def[1], verts[i].texc, pid);
		if(def[0] != def[1])
		{
			def[0] = i;
			return def;
		}
	}
	def[0] = def[1] = def[2] = NULLID;
	return def;
}

