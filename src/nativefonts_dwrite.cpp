
#include <nativefonts.h>

#ifdef NF_PLATFORM_DWRITE

#ifndef NF_SUPPORT_EXPLAIN_HR
#define NF_SUPPORT_EXPLAIN_HR 1
#endif

//#define COBJMACROS
//#include <initguid.h>
//#include <ole2.h>
//#include <shobjidl.h>
//#include "dwrite_c.h"
#include <dwrite.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <stdio.h>
#if NF_SUPPORT_EXPLAIN_HR
#include <comdef.h>
#endif
#include <stdlib.h>

// todo don't define here?
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "D3D10_1.lib")

typedef struct
{
	ID3D10Device1 * d3d_device;
	IDWriteFactory * dw_factory;
	ID2D1Factory1 * d2d_factory;

	ID3D10Texture2D * d3d_texture1;
	ID3D10Texture2D * d3d_texture2;

	ID2D1RenderTarget * d2d_rt;
	ID2D1SolidColorBrush * d2d_brush;

	size_t reference_counter;
} nf_context_t;

nf_context_t ctx = {0};

void nf_explain_hr(HRESULT hr, const char * msg)
{
	#if NF_SUPPORT_EXPLAIN_HR
	_com_error err(hr);
	NF_ERROR("%s %s\n", msg, err.ErrorMessage());
	#else
	NF_ON_ERROR("%s\n", msg);
	#endif
}

void nf_ctx_free();

int nf_ctx_init()
{
	if(ctx.reference_counter++)
		return 0;

	HRESULT hr = 0;

	if(FAILED(hr = D3D10CreateDevice1(
		NULL, D3D10_DRIVER_TYPE_HARDWARE,
		NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_1, D3D10_1_SDK_VERSION,
		&ctx.d3d_device)))
	{
		nf_explain_hr(hr, "can't create d3d10 device");
		nf_ctx_free();
		return -1;
	}

	D3D10_TEXTURE2D_DESC texdesc1;
	texdesc1.ArraySize = 1;
	texdesc1.BindFlags = D3D10_BIND_RENDER_TARGET;
	texdesc1.CPUAccessFlags = 0;
	texdesc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texdesc1.Width = NF_MAX_WIDTH;
	texdesc1.Height = NF_MAX_HEIGHT;
	texdesc1.MipLevels = 1;
	texdesc1.MiscFlags = 0;
	texdesc1.SampleDesc.Count = 1;
	texdesc1.SampleDesc.Quality = 0;
	texdesc1.Usage = D3D10_USAGE_DEFAULT;

	if(FAILED(hr = ctx.d3d_device->CreateTexture2D(
					&texdesc1,
					NULL,
					&ctx.d3d_texture1)))
	{
		nf_explain_hr(hr, "can't create d3d10 texture #1");
		nf_ctx_free();
		return -1;
	}

	IDXGISurface * dxgi_surface = NULL;
	if(FAILED(hr = ctx.d3d_texture1->QueryInterface(&dxgi_surface)))
	{
		nf_explain_hr(hr, "can't get dxgi interface");
		nf_ctx_free();
		return -1;
	}

	D3D10_TEXTURE2D_DESC texdesc2;
	texdesc2.ArraySize = 1;
	texdesc2.BindFlags = 0;
	texdesc2.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
	texdesc2.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texdesc2.Width = NF_MAX_WIDTH;
	texdesc2.Height = NF_MAX_HEIGHT;
	texdesc2.MipLevels = 1;
	texdesc2.MiscFlags = 0;
	texdesc2.SampleDesc.Count = 1;
	texdesc2.SampleDesc.Quality = 0;
	texdesc2.Usage = D3D10_USAGE_STAGING;

	if(FAILED(hr = ctx.d3d_device->CreateTexture2D(
					&texdesc2,
					NULL,
					&ctx.d3d_texture2)))
	{
		nf_explain_hr(hr, "can't create d3d10 texture #2");
		nf_ctx_free();
		return -1;
	}

	if(FAILED(hr = DWriteCreateFactory(
					DWRITE_FACTORY_TYPE_SHARED,
					//&IID_IDWriteFactory,
					__uuidof(IDWriteFactory),
					(IUnknown**)(&ctx.dw_factory))))
	{
		nf_explain_hr(hr, "can't create dwrite factory");
		nf_ctx_free();
		return -1;
	}

	D2D1_FACTORY_OPTIONS d2d1_options;
	d2d1_options.debugLevel = D2D1_DEBUG_LEVEL_WARNING;

	if(FAILED(hr = D2D1CreateFactory(
					D2D1_FACTORY_TYPE_SINGLE_THREADED,
					__uuidof(ID2D1Factory1),
					&d2d1_options,
					(void**)&ctx.d2d_factory)))
	{
		nf_explain_hr(hr, "can't create d2d1 factory");
		nf_ctx_free();
		return -1;
	}

	float ppi_x = 0.0f, ppi_y = 0.0f;
	ctx.d2d_factory->GetDesktopDpi(&ppi_x, &ppi_y);

	D2D1_RENDER_TARGET_PROPERTIES d2d1_props;
	d2d1_props.dpiX = ppi_x;
	d2d1_props.dpiY = ppi_y;
	d2d1_props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
	d2d1_props.pixelFormat.format = DXGI_FORMAT_UNKNOWN;
	d2d1_props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	d2d1_props.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	d2d1_props.usage = D2D1_RENDER_TARGET_USAGE_NONE;

	if(FAILED(hr = ctx.d2d_factory->CreateDxgiSurfaceRenderTarget(
					dxgi_surface,
					d2d1_props,
					&ctx.d2d_rt)))
	{
		nf_explain_hr(hr, "can't create d2d1 render target");
		nf_ctx_free();
		return -1;
	}

	if(FAILED(hr = ctx.d2d_rt->CreateSolidColorBrush(
					D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f),
					&ctx.d2d_brush)))
	{
		nf_explain_hr(hr, "can't create d2d1 solid brush");
		nf_ctx_free();
		return -1;
	}

	return 0;

}

void nf_ctx_free()
{
	if(!ctx.reference_counter)
		return;

	if(--ctx.reference_counter)
		return;

	if(ctx.d2d_brush)
	{
		ctx.d2d_brush->Release();
		ctx.d2d_brush = NULL;
	}

	if(ctx.d2d_rt)
	{
		ctx.d2d_rt->Release();
		ctx.d2d_rt = NULL;
	}

	if(ctx.d3d_texture2)
	{
		ctx.d3d_texture2->Release();
		ctx.d3d_texture2 = NULL;
	}

	if(ctx.d3d_texture1)
	{
		ctx.d3d_texture1->Release();
		ctx.d3d_texture1 = NULL;
	}

	if(ctx.d2d_factory)
	{
		ctx.d2d_factory->Release();
		ctx.d2d_factory = NULL;
	}
	if(ctx.dw_factory)
	{
		ctx.dw_factory->Release();
		ctx.dw_factory = NULL;
	}

	if(ctx.d3d_device)
	{
		ctx.d3d_device->Release();
		ctx.d3d_device = NULL;
	}
}

nf_system_info_t nf_system_info()
{
	float ppi_x = 0.0f, ppi_y = 0.0f;

	// instead of doing full initialization, let's just create a d2d1 factory
	if(!ctx.reference_counter)
	{
		HRESULT hr = 0;
		if(FAILED(hr = D2D1CreateFactory(
						D2D1_FACTORY_TYPE_SINGLE_THREADED,
						__uuidof(ID2D1Factory1),
						NULL,
						(void**)&ctx.d2d_factory)))
		{
			NF_ERROR("can't create d2d1 factory\n");
			ctx.d2d_factory = NULL;
		}
	}
	ctx.d2d_factory->GetDesktopDpi(&ppi_x, &ppi_y);
	if(!ctx.reference_counter && ctx.d2d_factory)
	{
		ctx.d2d_factory->Release();
		ctx.d2d_factory = NULL;
	}

	nf_system_info_t ret;
	ret.bitmap = NF_BITMAP_B8G8R8A8_UNORM_PMA;
	ret.max_width = NF_MAX_WIDTH;
	ret.max_height = NF_MAX_HEIGHT;
	ret.ppi_x = ppi_x;
	ret.ppi_y = ppi_y;
	return ret;
}

nf_font_t nf_font(const char * font_name, nf_font_params_t params)
{
	if(nf_ctx_init() < 0 || !font_name)
		return 0;

	size_t len = strlen(font_name) + 1;
	WCHAR * wtext = (WCHAR*)alloca(len * sizeof(WCHAR));
	size_t wlen = mbstowcs(wtext, font_name, len);
	if(wlen == (size_t)-1)
	{
		NF_ERROR("failed to convert text to wchar, text : '%s'\n", font_name);
		return 0;
	}

	IDWriteTextFormat * format = NULL;
	HRESULT hr = 0;
	if(FAILED(hr = ctx.dw_factory->CreateTextFormat(
					wtext,
					NULL,
					DWRITE_FONT_WEIGHT_NORMAL, // TODO
					DWRITE_FONT_STYLE_NORMAL, // TODO
					DWRITE_FONT_STRETCH_NORMAL, // TODO
					params.size_in_pt, // TODO maybe pt / 72 * 96 ?
					L"", // en-us ?
					&format)))
	{
		nf_explain_hr(hr, "can't create text format");
		return -1;
	}

	return (uintptr_t)format;
}

void nf_free(nf_font_t font)
{
	if(font)
	{
		IDWriteTextFormat * format = (IDWriteTextFormat*)font;
		format->Release();
	}

	nf_ctx_free();
}

int nf_print(void * bitmap, uint16_t w, uint16_t h,
	nf_font_t font, nf_feature_t * features, size_t features_count,
	nf_aabb_t * result_rect, const char * text, ...)
{
	if(!bitmap)
	{
		NF_ERROR("can't print with invalid bitmap\n");
		return -1;
	}

	if(!font)
	{
		NF_ERROR("can't print with invalid font\n");
		return -1;
	}

	if(!text)
	{
		NF_ERROR("can't print with invalid text\n");
		return -1;
	}

	// figure out text rendering settings
	HRESULT hr = 0;
	D2D1_COLOR_F bg_color = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
	D2D1_COLOR_F fg_color = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	DWRITE_WORD_WRAPPING text_wrap = DWRITE_WORD_WRAPPING_WRAP;
	DWRITE_TEXT_ALIGNMENT text_alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
	DWRITE_PARAGRAPH_ALIGNMENT parg_alignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
	D2D1_TEXT_ANTIALIAS_MODE text_aa_mode = D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
	float ppi_x = 0.0f, ppi_y = 0.0f;

	ctx.d2d_factory->GetDesktopDpi(&ppi_x, &ppi_y);

	size_t len = strlen(text) + 1;
	WCHAR * wtext = (WCHAR*)alloca(len * sizeof(WCHAR));
	size_t wlen = mbstowcs(wtext, text, len);
	if(wlen == (size_t)-1)
	{
		NF_ERROR("failed to convert text to wchar, text : '%s'\n", text);
		return -1;
	}

	IDWriteTextLayout * layout = NULL;
	if(FAILED(hr = ctx.dw_factory->CreateTextLayout(
					wtext, wlen,
					(IDWriteTextFormat*)font,
					w, h,
					&layout)))
	{
		nf_explain_hr(hr, "can't create dwrite text layout");
		return -1;
	}

	for(size_t i = 0; i < features_count; ++i)
	{
		DWRITE_TEXT_RANGE range;
		range.startPosition = features[i].range.start;
		range.length = features[i].range.end - features[i].range.start + 1;

		switch(features[i].type)
		{
		case NF_FEATURE_BOLD:
			layout->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, range);
			break;
		case NF_FEATURE_UNDERLINE:
			layout->SetUnderline(true, range);
			break;
		case NF_FEATURE_ITALIC:
			layout->SetFontStyle(DWRITE_FONT_STYLE_ITALIC, range);
			break;
		case NF_FEATURE_WRAP:
			text_wrap = DWRITE_WORD_WRAPPING_WRAP;
			break;
		case NF_FEATURE_NO_WRAP:
			text_wrap = DWRITE_WORD_WRAPPING_NO_WRAP;
			break;
		case NF_FEATURE_ALIGN_LEFT:
			text_alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
			break;
		case NF_FEATURE_ALIGN_CENTER:
			text_alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
			break;
		case NF_FEATURE_ALIGN_RIGHT:
			text_alignment = DWRITE_TEXT_ALIGNMENT_TRAILING;
			break;
		case NF_FEATURE_ALIGN_JUSTIFIED:
			text_alignment = DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
			break;
		case NF_FEATURE_ALIGN_PARAGRAPH_LEFT:
			parg_alignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			break;
		case NF_FEATURE_ALIGN_PARAGRAPH_CENTER:
			parg_alignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			break;
		case NF_FEATURE_ALIGN_PARAGRAPH_RIGHT:
			parg_alignment = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
			break;
		case NF_FEATURE_AA_DISABLED:
			text_aa_mode = D2D1_TEXT_ANTIALIAS_MODE_ALIASED;
			break;
		case NF_FEATURE_AA_WIN_CLEARTYPE:
			text_aa_mode = D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE;
			break;
		case NF_FEATURE_AA_WIN_GREYSCALE:
			text_aa_mode = D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE;
			break;
		case NF_FEATURE_PPI:
			ppi_x = features[i].ppi.x;
			ppi_y = features[i].ppi.y;
			break;
		case NF_FEATURE_COLOR_BG:
			bg_color = D2D1::ColorF(
				features[i].color.r,
				features[i].color.g,
				features[i].color.b,
				features[i].color.a);
			break;
		case NF_FEATURE_COLOR_TEXT:
			fg_color = D2D1::ColorF(
				features[i].color.r,
				features[i].color.g,
				features[i].color.b,
				features[i].color.a);
			break;
		default:
			break;
		}
	}

	layout->SetWordWrapping(text_wrap);
	layout->SetTextAlignment(text_alignment);
	layout->SetParagraphAlignment(parg_alignment);
	ctx.d2d_rt->SetDpi(ppi_x, ppi_y);
	ctx.d2d_rt->SetTextAntialiasMode(text_aa_mode);
	ctx.d2d_brush->SetColor(fg_color);

	// figure our result metrics
	// TODO does this call actually rasterizes text?
	DWRITE_TEXT_METRICS text_metrics;
	layout->GetMetrics(&text_metrics);
	float clip_x1 = text_metrics.left;
	float clip_y1 = text_metrics.top;
	float clip_x2 = text_metrics.left + text_metrics.width;
	float clip_y2 = text_metrics.top + text_metrics.height;
	clip_x1 = clip_x1 < 0 ? 0 : (clip_x1 >= w ? w - 1 : clip_x1);
	clip_y1 = clip_y1 < 0 ? 0 : (clip_y1 >= h ? h - 1 : clip_y1);
	clip_x2 = clip_x2 < 0 ? 0 : (clip_x2 >= w ? w - 1 : clip_x2);
	clip_y2 = clip_y2 < 0 ? 0 : (clip_y2 >= h ? h - 1 : clip_y2);
	float clip_w = clip_x2 - clip_x1 + 1.0f;
	float clip_h = clip_y2 - clip_y1 + 1.0f;
	nf_aabb_t aabb;
	aabb.x = clip_x1;
	aabb.y = clip_y1;
	aabb.w = clip_w;
	aabb.h = clip_h;
	if(result_rect)
		*result_rect = aabb;

	// render text
	ctx.d2d_rt->BeginDraw();
	ctx.d2d_rt->Clear(bg_color);
	ctx.d2d_rt->DrawTextLayout(D2D1::Point2F(), layout, ctx.d2d_brush);
	ctx.d2d_rt->EndDraw();
	layout->Release();
	layout = NULL;

	// read texture from d3d
	ctx.d3d_device->CopyResource(ctx.d3d_texture2, ctx.d3d_texture1);

	D3D10_MAPPED_TEXTURE2D mapped = {0};
	if(FAILED(hr = ctx.d3d_texture2->Map(0, D3D10_MAP_READ, 0, &mapped)))
	{
		nf_explain_hr(hr, "can't map d3d texture");
		return -1;
	}

	for(size_t j = aabb.y; j < aabb.y + aabb.h; ++j)
		// hardcoded BGRA8 format
		memcpy(
			(uint8_t*)bitmap + (j * w + aabb.x) * 4,
			(uint8_t*)mapped.pData + j * mapped.RowPitch + aabb.x,
			aabb.w * 4);

	ctx.d3d_texture2->Unmap(0);
	return 0;
}

#endif
