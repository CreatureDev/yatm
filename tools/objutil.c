#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "objutil.h"
#include "objvertlist.h"

/* This file handles the parsing of OBJ files and outputting
 * an equivilent CMB file (creature model buffer)
 * It operates on one file at a time
*/

#define TYPE_V ('v' | ' ' << 8)
#define TYPE_VN ('v' | 'n' << 8)
#define TYPE_VT ('v' | 't' << 8)
#define TYPE_F ('f' | ' ' << 8)

static FILE *fp;
static char *fname;
static unsigned int vcnt;
static unsigned int vtcnt;
static unsigned int vncnt;
static unsigned int fcnt;
static unsigned int fper;	//1 if triangulated, 2 if quads
static float *v;
static float *vt;
static float *vn;
static float *cmbv;
static float *cmbvt;
static float *cmbvn;
static unsigned int *cmbf;


void objinit(char *file)
{	
	if(fp)
		fclose(fp);
	fp = fopen(file, "r");
	if(!fp)
	{
		printf("failed to open file %s\n", file);
		exit(-1);
	}
	fname = file;
	vcnt = vtcnt = vncnt = fcnt = fper = 0;
	
	if(v)
		free(v);
	if(vt)
		free(vt);
	if(vn)
		free(vn);
	v = vt = vn = '\0';
}

void objcountvals()
{
	char lbuf[256];
	short *type = (short *) lbuf;
	if(vcnt | vtcnt | vncnt | fcnt)
		return;
	if(!fp)
		return;
	fseek(fp, 0, SEEK_SET);
	while(fgets(lbuf, 256, fp))
		switch(*type)
		{
		case TYPE_V:
			vcnt++;
			break;
		case TYPE_VN:
			vncnt++;
			break;
		case TYPE_VT:
			vtcnt++;
			break;
		case TYPE_F:
			fcnt++;
		default:
			break;
		}
}

void objpopulate()
{
	if(!fp)
		return;
	if(!(vcnt | vncnt | vtcnt))
		return;
	if(v || vn || vt)
		return;
	fseek(fp, 0, SEEK_SET);
	unsigned int cv, cvn, cvt, tmp;
	cv = cvn = cvt = 0;
	char lbuf[256];
	char *tbuf;
	short *type = (short *) lbuf;
	v = (float *) malloc(sizeof(float) * 3 * vcnt);
	vn = (float *) malloc(sizeof(float) * 3 * vncnt);
	vt = (float *) malloc(sizeof(float) * 2 * vtcnt);
	while(fgets(lbuf, 256, fp))
	{
		switch(*type)
		{
		case TYPE_V:
			tbuf = strtok(lbuf, " ");
			tmp = 3 * cv;
			tbuf = strtok('\0', " ");
			v[tmp] = (float) atof(tbuf);
			tbuf = strtok('\0', " ");
			v[tmp+1] = (float) atof(tbuf);
			tbuf = strtok('\0', " ");
			v[tmp+2] = (float) atof(tbuf);
			cv++;
			break;
		case TYPE_VN:
			tbuf = strtok(lbuf, " ");
            tmp = 3 * cvn;
            tbuf = strtok('\0', " ");
            vn[tmp] = (float) atof(tbuf);
            tbuf = strtok('\0', " ");
            vn[tmp+1] = (float) atof(tbuf);
            tbuf = strtok('\0', " ");
            vn[tmp+2] = (float) atof(tbuf);
            cvn++;
			break;
		case TYPE_VT:
			tbuf = strtok(lbuf, " ");
            tmp = 2 * cvt;
            tbuf = strtok('\0', " ");
            vt[tmp] = (float) atof(tbuf);
            tbuf = strtok('\0', " ");
            vt[tmp+1] = (float) atof(tbuf);
            cvt++;
			break;
		case TYPE_F:
			if(fper)
				break;
			tbuf = strtok(lbuf, " ");
			while((tbuf = strtok('\0', " ")))
			{
				if(strlen(tbuf))
					fper++;
			}
			fper -= 2;
		default:
			break;
		}
	}	
}


void objprint()
{
	printf("vcnt %u\nvtcnt %u\nvncnt %u\n", vcnt, vtcnt, vncnt);
	printf("fcnt %u\nfper %u\n", fcnt, fper);
}

void cmbpopulate()
{
	unsigned int cnt, i, *tmp;
	cnt = numunique();
	if(!cnt)
		return;
	if(cmbv)
		free(cmbv);
	if(cmbvt)
		free(cmbvt);
	if(cmbvn)
		free(cmbvn);
	cmbv = (float *) malloc(sizeof(float) * 3 * cnt);
	cmbvt = (float *) malloc(sizeof(float) * 2 * cnt);
	cmbvn = (float *) malloc(sizeof(float) * 3 * cnt);
	for(i = 0; i < cnt; i++)
	{
		tmp = getvert(i);
		if(tmp[0] == NULLID)
			cmbv[i*3] = cmbv[i*3+1] = cmbv[i*3+2] = 0.f;
		else
		{
			cmbv[i*3] = v[tmp[0]*3];
			cmbv[i*3+1] = v[tmp[0]*3+1];
			cmbv[i*3+2] = v[tmp[0]*3+2];
		}
		if(tmp[1] == NULLID)
			cmbvt[i*2] = cmbvt[i*2+1] = 0.f;
		else
		{
			cmbvt[i*2] = vt[tmp[1]*2];
			cmbvt[i*2+1] = vt[tmp[1]*2+1];
		}
		if(tmp[2] == NULLID)
			cmbvn[i*3] = cmbvn[i*3+1] = cmbvn[i*3+2] = 0.f;
		else
		{
			cmbvn[i*3] = vn[tmp[2]*3];
			cmbvn[i*3+1] = vn[tmp[2]*3+1];
			cmbvn[i*3+2] = vn[tmp[2]*3+2];
		}
		free(tmp);
	}
}

void objgenverts()
{
	char lbuf[256];
	char *tbuf;
	short *kek = (short *) lbuf;
	if(!(v || vn || vt))
        return;
    if(!(fcnt && fper && fp))
        return;
    vertlistinit(vcnt);
	fseek(fp, 0, SEEK_SET);
	while(fgets(lbuf, 256, fp))
	{
		if(*kek == TYPE_F)
		{
			tbuf = strtok(lbuf, " ");
			while((tbuf = strtok('\0', " ")))
				addvert(tbuf);
		}
	}
	idverts();
	cmbpopulate();
}

void objgenfaces()
{
	if(cmbf)
		return;
	char lbuf[256], *tbuf;
	short *kay;
	kay = (short *) lbuf;
	cmbf = (unsigned int *) malloc(sizeof(unsigned int) * 3 * fper * fcnt);
	unsigned int vid[4], i, j;
	fseek(fp, 0, SEEK_SET);
	i = 0;
	while(fgets(lbuf, 256, fp))
	{
		if(*kay == TYPE_F)
		{
			j = 0;
			vid[0] = vid[1] = vid[2] = vid[3] = NULLID;	
			tbuf = strtok(lbuf, " ");
			while(tbuf = strtok('\0', " "))
			{
				if(!strlen(tbuf))
					break;
				vid[j++] = getvertid(tbuf);
			}
			j = i * 3 * fper;
			cmbf[j++] = vid[0];
			cmbf[j++] = vid[1];
			cmbf[j++] = vid[2];
			if(fper > 1)
			{
				cmbf[j++] = vid[0];
				cmbf[j++] = vid[2];
				cmbf[j] = vid[3];
			}
			i++;
		}
	}
}

typedef struct cmbhd_s cmbhd_t;

struct cmbhd_s
{
	unsigned int vers;
	unsigned int vcnt;
	unsigned int fcnt;
};

void objwritecmb()
{
	unsigned int i;
	FILE *cmb;
	char *dname = (char *) malloc(strlen(fname) + 1);
	cmbhd_t hed;
	for(i = 0; i < strlen(fname) - 3; i++)
	{
		dname[i] = fname[i];
	}
	dname[i++] = 'c';
	dname[i++] = 'm';
	dname[i++] = 'b';
	dname[i] = '\0';
	cmb = fopen(dname, "w");
	hed.vers = 1;
	hed.vcnt = numunique();
	hed.fcnt = fcnt * fper;
	fwrite(&hed, sizeof(cmbhd_t), 1, cmb);
	fwrite(cmbv, sizeof(float), 3 * hed.vcnt, cmb);
	fwrite(cmbvt, sizeof(float), 2 * hed.vcnt, cmb);
	fwrite(cmbvn, sizeof(float), 3 * hed.vcnt, cmb);
	fwrite(cmbf, sizeof(unsigned int), 3 * hed.fcnt, cmb);
	free(dname);
	fclose(cmb);
}

void objclose()
{
	freevertlist();
	if(fp)
		fclose(fp);
	if(v)
		free(v);
	if(vt)
		free(vt);
	if(vn)
		free(vn);
	if(cmbv)
		free(cmbv);
	if(cmbvt)
		free(cmbvt);
	if(cmbvn)
		free(cmbvn);
	if(cmbf)
		free(cmbf);
}

