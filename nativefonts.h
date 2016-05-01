// codestyle :
// - C99
// - 80 columns max
// - tab = 4 spaces
// - snake_case

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_ON_ERROR
#define NF_ON_ERROR(msg, ...) fprintf(stderr, msg, __VA_ARGS__)
#endif

// ------------------------------------------------------------- font parameters
typedef enum {
	NF_WEIGHT_NORMAL = 0,
	NF_WEIGHT_BOLD,
	NF_WEIGHT_LIGHT,
	NF_WEIGHT_MAX = 0xff
} nf_font_weight_t;

typedef enum {
	NF_STYLE_NORMAL = 0,
	NF_STYLE_OBLIQUE,
	NF_STYLE_ITALIC,
	NF_STYLE_MAX = 0xff
} nf_font_style_t;

typedef enum {
	NF_STRETCH_NORMAL = 0,
	NF_STRETCH_EXPANDED,
	NF_STRETCH_CONDENSED,
	NF_STRETCH_MAX = 0xff
} nf_font_stretch_t;

typedef struct {
	float size_in_pt;
	uint8_t weight;
	uint8_t style;
	uint8_t stretch;
	uint8_t _reserved;
} nf_font_params_t;

// ---------------------------------------------------------- system information
#ifndef NF_MAX_WIDTH
#define NF_MAX_WIDTH 2048
#endif

#ifndef NF_MAX_HEIGHT
#define NF_MAX_HEIGHT 2048
#endif

typedef enum {
	NF_BITMAP_B8G8R8A8_UNORM_PMA = 0,
} nf_bitmap_t;

typedef struct {
	nf_bitmap_t bitmap;
	uint16_t max_width;
	uint16_t max_height;
	float ppi_x;
	float ppi_y;
} nf_system_info_t;

// --------------------------------------------------------------- text features
typedef enum {
	// emphasis
	NF_FEATURE_BOLD = 0,
	NF_FEATURE_UNDERLINE,
	NF_FEATURE_ITALIC,

	// align features are applied to whole text
	NF_FEATURE_ALIGN_LEFT,
	NF_FEATURE_ALIGN_CENTER,
	NF_FEATURE_ALIGN_RIGHT,
	NF_FEATURE_ALIGN_JUSTIFIED,
	NF_FEATURE_ALIGN_PARAGRAPH_LEFT,
	NF_FEATURE_ALIGN_PARAGRAPH_CENTER,
	NF_FEATURE_ALIGN_PARAGRAPH_RIGHT,

	// antialiasing is applied to whole text
	NF_FEATURE_AA_WIN_CLEARTYPE, // cleartype will not use alpha channel
	NF_FEATURE_AA_WIN_GREYSCALE, // greyscale uses PMA

	// ppi is applied to whole text
	NF_FEATURE_PPI_X,
	NF_FEATURE_PPI_Y,

	NF_FEATURE_MAX = 0xff
} nf_feature_type_t;

typedef struct {
	nf_feature_type_t type;
	size_t start, end;
	float value;
} nf_feature_t;

// ------------------------------------------------------------------- functions
typedef uintptr_t nf_font_t;

nf_system_info_t nf_system_info();

// returns 0 on error
nf_font_t nf_font(const char * font_name, nf_font_params_t params);
void nf_free(nf_font_t font);

// return < 0 on error
int nf_print(
	void * bitmap, uint16_t w, uint16_t h,
	nf_font_t font, nf_feature_t * features, size_t features_count,
	const char * text, ...
);

#ifdef __cplusplus
}
#endif
