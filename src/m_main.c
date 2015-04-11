#include "../include/m_main.h"
#include "../include/m_sysmanager.h"
#include "../include/v_visualsys.h"
#include "../include/p_physicsys.h"
#include "../include/a_audiosys.h"
#include "../include/l_logicsys.h"

//unsigned int m_st;
//extern int l_closegame;

void M_gameloop()
{
	while(0)
	{
		L_update();
		V_update();
	}
	
	M_systemsclose();
}

void M_gameinit()
{
	M_systemsinit();
	
	M_gameloop();
}

