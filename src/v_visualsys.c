#include "../include/m_state.h"
#include "../include/v_visualsys.h"
#include "../include/v_memory.h"
#include "../include/v_buffer.h"
#include "../include/v_model.h"
#include "../include/v_texture.h"
#include "../include/v_program.h"
#include "../include/v_entity.h"
#include "../include/v_loader.h"
#include "../include/v_default.h"

#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

GLFWwindow *v_frame;
void glClear(GLenum);
void glFinish();
void glClearColor(GLclampf, GLclampf, GLclampf, GLclampf);
void glBlendFunc(GLenum, GLenum);
void glEnable(GLenum);
void glGetIntegerv(GLenum, GLint *);
void glViewport(GLint, GLint, GLint, GLint);

int scrw;
int scrh;

void V_visualinit()
{
	int vp[4];
	if(!glfwInit())
	{
		printf("glfw init failed\n");
		exit(-1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	v_frame = glfwCreateWindow(1366, 768, "toasterdev v1s", glfwGetPrimaryMonitor(), '\0');
	if(!v_frame)
	{
		printf("failed to create OpenGL 3.3 context\n");
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(v_frame);
	glfwSwapInterval(0);
	glGetIntegerv(GL_VIEWPORT, vp);
	scrw = vp[2];
	scrh = vp[3];
	V_memoryinit();
	V_bufferinit();
	V_modelinit();
	V_textureinit();
	V_programinit();
	V_loaderinit();
	V_entityinit();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.1f, 0.3f, 0.3f, 1.f);
	glViewport(0, 0, scrw, scrh);
}

void V_usearg()
{
	//can either pass an argument index or the argument string
	//one common argument could be to load a file
	//this requires two strings, but if the first string is passed
	//the program won't know where to look for the second string
	//two versions of the function, one for single argument args
	//one for args that require two strings
}

void V_update()
{
	V_render();
}

void V_visualclose()
{
	V_entityclose();
	V_loaderclose();
	V_programclose();
	V_textureclose();
	V_modelclose();
	V_bufferclose();
	V_memoryclose();
	if(v_frame)
	{
		glfwTerminate();
		v_frame = '\0';
	}
}


void V_render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();
	glfwSwapBuffers(v_frame);
}

