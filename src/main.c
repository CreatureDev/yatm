#include "../include/m_main.h"

extern int m_argc;
extern char **m_argv;

int main(int argc, char **argv)
{
	m_argc = argc;
	m_argv = argv;
	M_gameinit();
	return 0;
}

