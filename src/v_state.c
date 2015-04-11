#include "../include/v_state.h"
#include "../include/m_enum.h"
#include <GL/gl.h>

/* This file handles the binding of buffers/array objects
 * and records the current state of OpenGL. This is responsible
 * for minimizing the number of OpenGL state changes.
*/

/* store texture and buffer indecies as well as currently
 * bound buffers (VERTEX_ARRAY_BUFFER, TEXTURE_2D_BUFFER, etc.)
 * ?
 *
*/

//static unsigned int vabuf;
//static unsigned int eabuf;
//static unsigned int unibuf;
//static unsigned int ppbuf;
//static unsigned int pubuf;

void glBindBuffer(GLenum, GLuint);

void V_bindbuffer(unsigned int buf, Menum typ)
{

}

void V_bindtexture(unsigned int tex, Menum typ)
{

}


void V_bindtextureindex(unsigned int tex, unsigned int id)
{

}

