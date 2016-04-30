#pragma once

//typedef struct nf_context_t nf_context_t;
//typedef struct nf_font_t nf_font_t;

#include "tigr.h"

#ifdef __cplusplus
extern "C" {
#endif

int nf_init();
int nf_deinit();

int nf_draw(Tigr * bitmap, const char * text);

//int nf_free();

#ifdef __cplusplus
}
#endif
