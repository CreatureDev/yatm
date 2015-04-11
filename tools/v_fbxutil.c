#include "../include/v_fbxutil.h"
#include <stdio.h>
#include <stdlib.h>

static char bytes[256];

void V_readzipped(char c, FILE *fp)
{

}

void V_readnode(FILE *fp)
{
	unsigned int tmp[3], i, j, k;
	char nl;
	short v0;
	char v1;
	int v2;
	float v3;
	double v4;
	long v5;
	fread(tmp, sizeof(unsigned int), 3, fp);
	printf("node description\n");
	printf("EndOffset: %u\n", tmp[0]);
	printf("NumProperties: %u\n", tmp[1]);
	printf("PropertyListLen: %u\n", tmp[2]);
	fread(&nl, 1, 1, fp);
	printf("NameLen: %d\n", nl);
	fread(bytes, 1, nl, fp);
	bytes[nl] = '\0';
	printf("Name: %s\n", bytes);
	for(i = 0; i < tmp[1]; i++)
	{
		fread(&nl, 1, 1, fp);
		switch(nl)
		{
		case 'Y':
			fread(&v0, 2, 1, fp);
			printf("short val: %d\n", v0);
			break;
		case 'C':
			fread(&v1, 1, 1, fp);
			printf("bool val: %d\n", v1);
			break;
		case 'I':
			fread(&v2, 4, 1, fp);
			printf("int val: %d\n", v2);
			break;
		case 'F':
			fread(&v3, 4, 1, fp);
			printf("float val: %f\n", v3);
			break;
		case 'D':
					
			break;
		case 'L':
			
			break;
		case 'f':
			fread(&k, 4, 1, fp);
			fread(&j, 4, 1, fp);
			if(j)
				V_readzipped('f', fp);
			break;
		case 'd':
			fread(&k, 4, 1, fp);
			fread(&j, 4, 1, fp);
		if(j)
			V_readzipped('d', fp);
		else
			
			break;
		case 'l':
			fread(&k, 4, 1, fp);
			fread(&j, 4, 1, fp);
		if(j)
			V_readzipped('l', fp);
			break;
		case 'i':
			fread(&k, 4, 1, fp);
			fread(&j, 4, 1, fp);
			if(j)
			
			break;
		case 'b':
			fread(&k, 4, 1, fp);
			fread(&j, 4, 1, fp);
			if(j)
			
			break;
		case 'S':
			fread(&k, 4, 1, fp);
			fread(&j, 4, 1, fp);
			if(j)
			
			break;
		case 'R':
			fread(&k, 4, 1, fp);
			fread(&j, 4, 1, fp);
			if(j)
			
			break;
		default:
			printf("what the fuck kind of data is %c?\n", nl);
		}
	}

//	while(ftell(fp) < tmp[0]);
//		V_readnode(fp);
}

void V_fbxtest()
{	
//	unsigned int tmp;
	FILE *fp = fopen("../bin/cube.fbx", "r");
	if(!fp)
	{
		printf("couldn't open cube.fbx\n");
		return;
	}
	fseek(fp, 26, SEEK_SET);
	V_readnode(fp);
	
	fclose(fp);
}
