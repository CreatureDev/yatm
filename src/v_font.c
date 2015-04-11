#include "../include/v_font.h"
#include "../include/v_program.h"

#include <stdio.h>
#include <stdlib.h>

#include <ft2build.h>
#include FT_FREETYPE_H

static FT_Library ftlib;

#include <GL/gl.h>

extern char M_basedir[];

void V_fontinit()
{
	if(FT_Init_FreeType(&ftlib))
	{
		printf("failed to load freetype\n");
		exit(-1);
	}
}

void V_fontclose()
{
	if(ftlib)
		FT_Done_FreeType(ftlib);
	ftlib = 0;
}

