#ifndef __V_TYPES_H__
#define __V_TYPES_H__

typedef struct model_s model_t;
typedef struct texture_s texture_t;
typedef struct font_s font_t;
typedef struct shader_s shader_t;
typedef struct program_s program_t;

struct model_s
{
	char *file;
	unsigned int *vcnt;
	unsigned int *fcnt;
	void **vrt;
	void **tex;
	void **nrm;
	void **fac;
};

struct texture_s
{
	char *file;
	unsigned int *name;
	unsigned int *w;
	unsigned int *h;
	unsigned int *format;
};

struct shader_s
{
	char *file;
	unsigned int *name;
};

struct font_s
{
	char *file;
	unsigned int *name;
};

struct program_s
{
	unsigned int *name;
	unsigned int *vsh;
	unsigned int *fsh;
//	unsigned int *gsh;
};

#define VPOS_INDEX 1
#define VTEX_INDEX 2
#define VNRM_INDEX 3
#define MDLTEX_UNIT 4

#endif

