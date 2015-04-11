#include "../include/v_loader.h"
#include "../include/v_memory.h"
#include "../include/v_buffer.h"
#include "../include/v_program.h"
#include "../include/v_state.h"
#include "../include/v_types.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

#define BIN_DIR "/bin"
#define SHADER_DIR "/shader"
#define VERTS_PER_FACE 3
#define CMB_HEAD_LEN (sizeof(unsigned int) * 3)
#define CMB_VERT_LEN(x) (x * sizeof(float) * 6 + x * sizeof(float) * 2)

extern char M_basedir[];

#define MAX_FBUFF_SIZE 0x100000

void *fbuff = '\0';

/* This file is responsible for the loading of visual
 * assets into process memory and passing the data along
 * with any additional description or usage information to
 * the appropriate OpenGL memory managers.
*/

/* static table_t mesht = {0, '\0'};
 
 * static table_t shdrt = {0, '\0'};
*/

GLuint glCreateShader(GLenum);
void glShaderSource(GLuint, GLsizei, GLchar **, GLsizei *);
void glCompileShader(GLuint);
void glGetShaderiv(GLuint, GLenum, GLint *);
void glDeleteShader(GLuint);
void glGetShaderInfoLog(GLuint, GLsizei, GLsizei *, GLchar *);

/* This function loads model data into a file buffer. Pointers
 * to specific segments of buffer are then generated and passed
 * to v_buffer.c which fills the data into the buffer corresponding
 * to the type of model.
 *
 * The OpenGL buffer storage process sets the value for a collection
 * of pointers whose count is determined by the model type which are
 * then stored as a reference to the model for all entities that make
 * use of it.
*/


/* loads the given model
 * if the model already has vrt, tex, nrm, elm ptrs it will
 * not load cuz it must already be loaded dummy
 * const cuz it shouldn't change where the pointers are pointin
*/
void V_loadmodel(const model_t mod)
{
	unsigned int i;
	char *path;
	FILE *fl;
	void *vdat, *fdat;
	if(*mod.vrt || *mod.tex || *mod.nrm || *mod.fac)
		return;
	i = strlen(M_basedir);
	i += strlen(BIN_DIR);
	i += strlen(mod.file);
	path = V_alloc(i + 1);
	sprintf(path, "%s%s/%s", M_basedir, BIN_DIR, mod.file);
	printf("%s\n", path);
	fl = fopen(path, "r");
	V_free(path);
	if(!fl)
		return;

	fseek(fl, 0, SEEK_END);
	i = ftell(fl);
	fseek(fl, 0, SEEK_SET);
	if(!i)
	{
		fclose(fl);
		return;
	}
	fbuff = V_alloc(i + 1);
	fread(fbuff, i, 1, fl);
	path = (char *) fbuff;
	path[i] = '\0';
	i = *(unsigned int *) fbuff;
	if(i != 1)
	{
		printf("unknown cmb version %u\n", i);
		V_free(fbuff);
		fclose(fl);
		return;
	}
	*mod.vcnt = *(unsigned int *) (fbuff + sizeof(unsigned int));
	*mod.fcnt = *(unsigned int *) (fbuff + sizeof(unsigned int) * 2);
	*mod.fcnt *= VERTS_PER_FACE;
	vdat = fbuff + CMB_HEAD_LEN;
	fdat = fbuff + CMB_HEAD_LEN + CMB_VERT_LEN(*mod.vcnt);
	V_storebuffermodel(mod, vdat, fdat);
	
	V_free(fbuff);
	fbuff = '\0';
	fclose(fl);	
}

/* This function loads shader source code into a file buffer,
 * determines general buffer usage, and passes it accordingly
 * to v_program.c to generate and compile said shader. It is
 * then stored along with a reference so that it may be linked
 * and used by dependant OpenGL programs.
*/

void V_loadshader(const shader_t shd)
{
	unsigned long i;
	int j;
	FILE *shfl;
	char buff[256];
	char *src;
	if(*shd.name)
		return;
	i = strlen(shd.file);
	switch(shd.file[i-1])
	{
	case 'v':
		*shd.name = glCreateShader(GL_VERTEX_SHADER);
		break;
	case 'f':
		*shd.name = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	default:
		return;
	}
	sprintf(buff, "%s%s/%s", M_basedir, SHADER_DIR, shd.file);
	shfl = fopen(buff, "r");
	if(!shfl)
	{
		printf("failed to open shader\npath: %s\n", buff);
		glDeleteShader(*shd.name);
		*shd.name = 0;
		return;
	}
	fseek(shfl, 0, SEEK_END);
	i = ftell(shfl);
	fbuff = V_alloc(i + 1);
	if(!fbuff)
	{
		printf("failed to allocate buffer for shader code\n");
		glDeleteShader(*shd.name);
		*shd.name = 0;
		return;
	}
	src = (char *) fbuff;
	fseek(shfl, 0, SEEK_SET);
	fread(src, i, 1, shfl);
	src[i] = '\0';
	fclose(shfl);
	glShaderSource(*shd.name, 1, &src, 0);
	glCompileShader(*shd.name);
	glGetShaderiv(*shd.name, GL_COMPILE_STATUS, &j);
	if(j == GL_FALSE)
	{
		printf("shader %s failed to compile\n", shd.file);

		glGetShaderInfoLog(*shd.name, 256, &j, buff);
		printf("%s\n", buff);

		glDeleteShader(*shd.name);
		*shd.name = 0;
	}
	V_free(fbuff);
	fbuff = '\0';
}

/* This function loads texture data into a file buffer and passes
 * a pointer and description of it to v_buffer.c (v_texture.c?)
 * which then stores the texture in OpenGL along with a reference
 * to be used by any model that requests it.
*/

void V_loadtexture(const texture_t tex)
{

}

void V_unloadmodel(const model_t mod)
{
	V_removebuffermodel(mod);
	*mod.vrt = *mod.tex = *mod.nrm = *mod.fac = '\0';
	*mod.vcnt = *mod.fcnt = 0;
}

void V_unloadshader(const shader_t shd)
{
	glDeleteShader(*shd.name);
	*shd.name = 0;
}

void V_unloadtexture(const texture_t tex)
{
	
}

void V_loaderinit()
{

}

void V_loaderclose()
{
	
}


