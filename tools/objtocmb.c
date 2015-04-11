#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "objutil.h"

#define OBJ ('.' | 'o' << 8 | 'b' << 16 | 'j' << 24)

void usage(char *prg)
{
	printf("%s <path/to/obj>\n", prg);
	exit(-1);
}

int main(int argc, char **argv)
{
	if(argc < 2)
		usage(argv[0]);
	int i = strlen(argv[1]);
	unsigned int *tmp = (unsigned int *) &argv[1][i - 4];
	if(*tmp != OBJ)
	{
		printf("%s is not an obj file\n", argv[1]);
		exit(-1);
	}
	
	objinit(argv[1]);
	objcountvals();
	objpopulate();
//	objprint();
	objgenverts();
	objgenfaces();
	objwritecmb();
	objclose();
	return 0;
}

