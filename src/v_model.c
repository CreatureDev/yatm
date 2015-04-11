#include "../include/v_model.h"
#include "../include/v_memory.h"
#include "../include/v_loader.h"
#include "../include/v_types.h"
#include "../include/m_hashlib.h"
#include "../include/v_buffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BIN_DIR "/bin"
#define MODEL_FILE_EXT ".cmb"

/* This file is in charge of managing all models used by entities.
 * It does not directly set or modify buffer data, but is responsible
 * for maintaining references to models within buffers as well as
 * calling to load and unload model data from the buffers as needed.
 *
 * It contains one reference to every available model and esures
 * only one instance of a given model exists in OpenGL buffer space
 * at any time.
*/

static unsigned int cnt;
static void *mdres = '\0';
static char *files = '\0';
static unsigned int *vcnts = '\0';
static unsigned int *fcnts = '\0';
static void **vrts = '\0';
static void **texs = '\0';
static void **nrms = '\0';
static void **facs = '\0';

extern char M_basedir[];

void V_bindmodel(unsigned int mdl)
{
	model_t *mdls = (model_t *) mdres;
	if(!mdls[mdl].vcnt)
		V_loadmodel(mdls[mdl]);
	V_bindverts(mdls[mdl]);
}

unsigned int V_getmodel(const char *mdl)
{
	unsigned int mid;
	model_t *mdls = (model_t *) mdres;
	for(mid = 1; mid <= cnt; mid++)
		if(!strcmp(mdl, mdls[mid].file))
			break;
	if(mid > cnt)
		return 1;
	if(*mdls[mid].vcnt)
		return mid;
	
	V_loadmodel(mdls[mid]);
	return mid;
}

void V_modelinit()
{
	char buff[256];
	long i, j, k;
	FILE *ls;
	model_t *tmp;
	sprintf(buff, "(cd %s%s && ls *%s) | wc -c", M_basedir, BIN_DIR, MODEL_FILE_EXT);
	ls = popen(buff, "r");
	fgets(buff, 256, ls);
	i = atoi(buff);
	pclose(ls);
	files = (char *) V_alloc(i);
	files[0] = '\0';
	sprintf(buff, "(cd %s%s && ls *%s)", M_basedir, BIN_DIR, MODEL_FILE_EXT);
	ls = popen(buff, "r");
	cnt = 0;
	while(fgets(buff, 256, ls))
	{
		cnt++;
		strcat(files, buff);
	}
	pclose(ls);
	mdres = V_alloc(sizeof(model_t) * (cnt + 1));
	vcnts = (unsigned int *) V_alloc(sizeof(unsigned int) * (cnt + 1));
	fcnts = (unsigned int *) V_alloc(sizeof(unsigned int) * (cnt + 1));
	vrts = (void **) V_alloc(sizeof(void *) * (cnt + 1));
	texs = (void **) V_alloc(sizeof(void *) * (cnt + 1));
	nrms = (void **) V_alloc(sizeof(void *) * (cnt + 1));
	facs = (void **) V_alloc(sizeof(void *) * (cnt + 1));
	k = 1;
	tmp = (model_t *) mdres;
	tmp[k++].file = files;
	for(j = 0; j < i; j++)
	{
		if(files[j] == '\n')
		{
			files[j] = '\0';
			if(k <= cnt)
				tmp[k++].file = &files[j+1];
		}
	}
	for(j = 1; j <= cnt; j++)
	{
		tmp[j].vcnt = &vcnts[j];
		tmp[j].fcnt = &fcnts[j];
		tmp[j].vrt = &vrts[j];
		tmp[j].tex = &texs[j];
		tmp[j].nrm = &nrms[j];
		tmp[j].fac = &facs[j];
		*tmp[j].vcnt = 0;
		*tmp[j].fcnt = 0;
		*tmp[j].vrt = 0;
		*tmp[j].tex = 0;
		*tmp[j].nrm = 0;
		*tmp[j].fac = 0;
	}
}

void V_modelclose()
{
	if(mdres)
		V_free(mdres);
	if(files)
		V_free(files);
	if(vcnts)
		V_free(vcnts);
	if(fcnts)
		V_free(fcnts);
	if(vrts)
		V_free(vrts);
	if(texs)
		V_free(texs);
	if(nrms)
		V_free(nrms);
	if(facs)
		V_free(facs);
	mdres = files = '\0';
	vrts = texs = nrms = facs = '\0';
	fcnts = vcnts = '\0';
}

