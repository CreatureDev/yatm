#ifndef __V_LOADER_H__
#define __V_LOADER_H__

typedef struct model_s model_t;
typedef struct texture_s texture_t;
typedef struct shader_s shader_t;

void V_loaderinit();
void V_loaderclose();

void V_loadmodel(const model_t);
void V_loadshader(const shader_t);
void V_loadtexture(const texture_t);

void V_unloadmodel(const model_t);
void V_unloadshader(const shader_t);
void V_unloadtexture(const texture_t);

#endif

