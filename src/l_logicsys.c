#include "../include/m_message.h"
#include "../include/l_logicsys.h"
#include "../include/l_memory.h"
#include "../include/l_gui.h"
#include "../include/l_input.h"
#include "../include/l_commands.h"
#include <GLFW/glfw3.h>

void L_logicinit()
{
	L_memoryinit();
}

void L_loadconsole()
{

}

void L_update()
{
	glfwPollEvents();
}

void L_logicclose()
{	
	L_memoryclose();
}

