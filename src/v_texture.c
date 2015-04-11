#include "../include/v_texture.h"
#include "../include/v_memory.h"
#include "../include/v_types.h"

#define TEXRES_SIZE 0x1000

/* This file is in charge of managing all available textures
 * used by the visual system. It maintains one reference to
 * each texture and is in charge of maintaining reference 
 * to their location within OpenGL buffer space, and loading
 * or unloading them as necessary.
 *
 * It ensures that only one instance of any given texture
 * exists within OpenGL buffer space at any time.
*/

//static void *tbl;
//static void *buf;

static void *texres;

void V_textureinit()
{
	if(!texres)
		texres = V_alloc(TEXRES_SIZE);
}

void V_textureclose()
{
	if(texres)
		V_free(texres);
	texres = '\0';
}

