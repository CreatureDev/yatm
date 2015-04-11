#include "../include/v_program.h"
#include "../include/v_types.h"
#include "../include/v_loader.h"
#include "../include/v_memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

#define SHD_DIR "/shader"
#define VRT_EXT ".*v"
#define FRG_EXT ".*f"

GLuint glCreateProgram();
void glUseProgram(GLuint);
void glAttachShader(GLuint prg, GLuint shd);
void glLinkProgram(GLuint);
void glDeleteProgram(GLuint);
void glDeleteShader(GLuint);
void glGetProgramiv(GLuint, GLenum, GLint *);
GLuint glGetUniformLocation(GLuint, const GLchar *);
GLuint glGetAttribLocation(GLuint, const GLchar *);
void glBindAttribLocation(GLuint, GLuint, const GLchar *);

/* This file is in charge of managing the shaders and
 * OpenGL programs used by the entities. It calls to
 * load shaders and create programs or remove them
 * as needed. It ensures only one instance of any shader
 * is loaded and compiled at any time and ensures any new
 * program that is created is unique.
*/


/* The way space is allocated for programs is very primitive.
 * It allocates enough room to hold each possible vertex/fragment
 * shader combination, but many of these combinations do not 
 * work, as they have different uniforms or pass different values
 * through to the fragment shader.
*/


//static void *vtbl;
//static void *gtbl;
//static void *ftbl;
//static void *ptbl;

static unsigned int vshcnt;
static unsigned int fshcnt;
static unsigned int prgcnt;

static void *shres;
static void *prres;
static char *shfls;

static unsigned int *shnms;
static unsigned int *prnms;

static unsigned int *vshs;
static unsigned int *fshs;

extern char M_basedir[];

void V_useprogram(unsigned int prg)
{
	glUseProgram(prnms[prg]);
}

static unsigned int V_genprogram(shader_t vshd, shader_t fshd)
{
	unsigned int prg;
	int st;
	if(!(*vshd.name && *fshd.name))
	{
		printf("at least one of these shaders is broken\n");
		return 0;
	}
	prg = glCreateProgram();
	glAttachShader(prg, *vshd.name);
	glAttachShader(prg, *fshd.name);
	glLinkProgram(prg);
	glGetProgramiv(prg, GL_LINK_STATUS, &st);
	if(st == GL_FALSE)
	{
		printf("failed to create program with %s and %s\n", vshd.file, fshd.file);
		glDeleteProgram(prg);
		return 0;
	}
	return prg;
}

//linear time coplexity
//make it a binary search
unsigned int V_getprogram(const char *vsh, const char *fsh)
{
	unsigned int vid, fid, pid;
	shader_t *shd = (shader_t *) shres;
	program_t *prg = (program_t *) prres;
	for(vid = 1; vid <= vshcnt; vid++)
		if(!strcmp(vsh, shd[vid].file))
			break;

	if(vid > vshcnt)
		return 0;

	for(fid = 1; fid <= fshcnt; fid++)
		if(!strcmp(fsh, shd[vshcnt + fid].file))
			break;

	if(fid > fshcnt)
		return 0;
	
	for(pid = 1; pid <= prgcnt; pid++)
		if(*prg[pid].vsh == vid && *prg[pid].fsh == fid)
			return pid;
	
	for(pid = 1; pid <= prgcnt; pid++)
		if(!(*prg[pid].vsh || *prg[pid].fsh))
			break;
	
	if(pid > prgcnt)
		return 0;
	if(!*shd[vid].name)
		V_loadshader(shd[vid]);
	if(!*shd[vshcnt+fid].name)
		V_loadshader(shd[vshcnt+fid]);
	
	*prg[pid].vsh = vid;
	*prg[pid].fsh = fid;
	*prg[pid].name = V_genprogram(shd[vid], shd[vshcnt+fid]);
	return pid;
}

void V_defaultattribloc(unsigned int pid)
{
	register unsigned int prog = prnms[pid];
	if(prog)
		return;
	glBindAttribLocation(prog, VPOS_INDEX, "vpos");
	glBindAttribLocation(prog, VTEX_INDEX, "vtex");
	glBindAttribLocation(prog, VNRM_INDEX, "vnrm");
}

unsigned int V_getuniformlocation(unsigned int pid, const char *uni)
{
	return glGetUniformLocation(prnms[pid], uni);
}

unsigned int V_getattriblocation(unsigned int pid, const char *att)
{
	return glGetAttribLocation(prnms[pid], att);
}

void V_removeprogram(unsigned int pid)
{
	glDeleteProgram(prnms[pid]);
	prnms[pid] = 0;
	glDeleteShader(shnms[vshs[pid]]);
	vshs[pid] = 0;
	glDeleteShader(shnms[fshs[pid] + vshcnt]);
	fshs[pid] = 0;
}

void V_programinit()
{
	char buff[256];
	char *tmp;
	long i = 0;
	FILE *ls;
	shader_t *shd;
	program_t *prg;
	sprintf(buff, "(cd %s%s && ls *%s *%s) | wc -c", 
		M_basedir, SHD_DIR, VRT_EXT, FRG_EXT);
	
	ls = popen(buff, "r");
	if(!ls)
	{
		printf("popen on command %s failed\n", buff);
		exit(-1);
	}
	fgets(buff, 256, ls);
	pclose(ls);
	i = atoi(buff);
	if(!i)
	{
		printf("total shader file name length failure\n");
		exit(-1);
	}
	shfls = (char *) V_alloc(i + 1);
	sprintf(buff, "(cd %s%s && ls *%s | wc -l && ls *%s | wc -l)", 
		M_basedir, SHD_DIR, VRT_EXT, FRG_EXT);

	ls = popen(buff, "r");
	if(!ls)
	{
		printf("popen on command \"%s\" failed\n", buff);
		exit(-1);
	}
	fgets(buff, 256, ls);
	vshcnt = atoi(buff);
	fgets(buff, 256, ls);
	fshcnt = atoi(buff);
	pclose(ls);
	if(!(vshcnt && fshcnt))
	{
		printf("shader file count failure\n");
		exit(-1);
	}
	
	sprintf(buff, "(cd %s%s && ls *%s && ls *%s)",
		M_basedir, SHD_DIR, VRT_EXT, FRG_EXT);
	
	ls = popen(buff, "r");
	if(!ls)
	{
		printf("popen on command \"%s\" failed\n", buff);
		exit(-1);
	}
	shfls[0] = '\0';
	while(fgets(buff, 256, ls))
	{
		strcat(shfls, buff);
	}
	pclose(ls);

	shres = V_alloc(sizeof(shader_t) * (vshcnt + fshcnt + 1));
	shnms = V_alloc(sizeof(unsigned int) * (vshcnt + fshcnt + 1));
	if(!(shres && shnms))
	{
		printf("failed to allocate shader manager\n");
		exit(-1);
	}
	shd = (shader_t *) shres;
	tmp = shfls;
	i = 1;
	shd++;
	while(i <= vshcnt)
	{
		shd->file = tmp;
		shd->name = &shnms[i];
		*shd->name = 0;
		while(*tmp != '\n')
			tmp++;
		*tmp = '\0';
		shd++;
		i++;
		tmp++;
	}
	i = 1;
	while(i <= fshcnt)
	{
		shd->file = tmp;
		shd->name = &shnms[i + vshcnt];
		*shd->name = 0;
		while(*tmp != '\n')
			tmp++;
		*tmp = '\0';
		shd++;
		i++;
		tmp++;
	}

	//need to improve how program count is generated
	//not all vertex/fragment shader pairs will be usable

	prgcnt = vshcnt * fshcnt;
	prres = V_alloc(sizeof(program_t) * (prgcnt + 1));
	prnms = (unsigned int *) V_alloc(sizeof(unsigned int) * (prgcnt + 1));
	vshs = (unsigned int *) V_alloc(sizeof(unsigned int) * (prgcnt + 1));
	fshs = (unsigned int *) V_alloc(sizeof(unsigned int) * (prgcnt + 1));
	if(!(prres && prnms && vshs && fshs))
	{
		printf("Failed to allocate program manager\n");
		exit(-1);
	}
	prg = (program_t *) prres;
	for(i = 1; i <= prgcnt; i++)
	{
		prg[i].name = &prnms[i];
		prg[i].vsh = &vshs[i];
		prg[i].fsh = &fshs[i];
		prnms[i] = vshs[i] = fshs[i] = 0;
	}
	prnms[0] = vshs[0] = fshs[0] = shnms[0] = 0;
	shd = (shader_t *) shres;
	shd->name = &shnms[0];
	shd->file = '\0';
}

void V_programclose()
{
	if(shres)
		V_free(shres);
	if(prres)
		V_free(prres);
	if(shfls)
		V_free(shfls);
	if(shnms)
		V_free(shnms);
	if(prnms)
		V_free(prnms);
	if(vshs)
		V_free(vshs);
	if(fshs)
		V_free(fshs);
	shres = prres = shfls = '\0';
	shnms = prnms = vshs = fshs = '\0';
}

