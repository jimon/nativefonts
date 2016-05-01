
#include <nativefonts.h>

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
//#include <d2d1_1helper.h>
//#include <wincodec.h>
#include <stdio.h>
#if NF_SUPPORT_EXPLAIN_HR
#include <comdef.h>
#endif
#include <cstdlib>
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
	NF_ON_ERROR("%s %s\n", msg, err.ErrorMessage());
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
			NF_ON_ERROR("can't create d2d1 factory\n");
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
	//IDWriteTextFormat * format;

	if(nf_ctx_init() < 0)
		return 0;

	return 0;
}

void nf_free(nf_font_t font)
{
	nf_ctx_free();
}

int nf_print(
	void * bitmap, uint16_t w, uint16_t h,
	nf_font_t font, nf_feature_t * features, size_t features_count,
	const char * text, ...
)
{
	return false;
}


/*
typedef struct
{
	IDWriteFactory * factory;
	IDWriteTextFormat * format;
	ID2D1Factory1 * d2d_factory;

	ID3D10Device1 *d3d10_device;

	ID3D10Texture2D * texture;
	ID3D10Texture2D * texture2;

	IDXGISurface * surface;

	ID2D1RenderTarget * rt;
	ID2D1SolidColorBrush * solid_brush;

} dwrite_t;

dwrite_t dw;

FLOAT ConvertPointSizeToDIP(FLOAT points)
{
	return (points/72.0f)*96.0f;
}

int nf_init()
{
	HRESULT hr = 0;

	if(FAILED(hr = D3D10CreateDevice1(
		NULL,
		D3D10_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_1,
		D3D10_1_SDK_VERSION,
		&dw.d3d10_device)))
	{
		printf("can't into d3d10\n");
		return -1;
	}

	// Allocate a offscreen D3D surface for D2D to render our 2D content into
	D3D10_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.Width = 1000;
	texDesc.Height = 800;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D10_USAGE_DEFAULT;

	D3D10_TEXTURE2D_DESC texDesc2;
	texDesc2.ArraySize = 1;
	texDesc2.BindFlags = 0;
	texDesc2.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
	texDesc2.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc2.Width = 1000;
	texDesc2.Height = 800;
	texDesc2.MipLevels = 1;
	texDesc2.MiscFlags = 0;
	texDesc2.SampleDesc.Count = 1;
	texDesc2.SampleDesc.Quality = 0;
	texDesc2.Usage = D3D10_USAGE_STAGING;

	if(FAILED(hr = dw.d3d10_device->CreateTexture2D(&texDesc, NULL, &dw.texture)))
	{
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		printf("can't into d3d10 texture %s\n", errMsg);
		return -1;
	}

	if(FAILED(hr = dw.d3d10_device->CreateTexture2D(&texDesc2, NULL, &dw.texture2)))
	{
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		printf("can't into d3d10 texture2 %s\n", errMsg);
		return -1;
	}

	if(FAILED(hr = dw.texture->QueryInterface(&dw.surface)))
	{
		printf("can't into dxgi surface\n");
		return -1;
	}

	if(FAILED(hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
									   //&IID_IDWriteFactory,
									   __uuidof(IDWriteFactory),
									   (IUnknown**)(&dw.factory))))
	{
		nf_deinit();
		return -1;
	}

	if(FAILED(hr = dw.factory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		12.0f,
		L"", // en-us ?
		&dw.format)))
	{
		nf_deinit();
		return -1;
	}

	dw.format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	dw.format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

	if(FAILED(hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, (void**)&dw.d2d_factory)))
	{
		// todo
		printf("nope1\n");
		return -1;
	}

	float dpi_x, dpi_y;
	dw.d2d_factory->GetDesktopDpi(&dpi_x, &dpi_y);

	D2D1_RENDER_TARGET_PROPERTIES props;
	props.dpiX = dpi_x;
	props.dpiY = dpi_y;
	props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
	props.pixelFormat.format = DXGI_FORMAT_UNKNOWN;
	props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	props.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	props.usage = D2D1_RENDER_TARGET_USAGE_NONE;

	if(FAILED(hr = dw.d2d_factory->CreateDxgiSurfaceRenderTarget(dw.surface, props, &dw.rt)))
	{
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		printf("failed2 %s\n", errMsg);
		return -1;
	}


	if(FAILED(hr = dw.rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &dw.solid_brush)))
	{
		printf("failed to create d2d brush");
		return -1;
	}

	//dw.rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
	dw.rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	return 0;
}

int nf_deinit()
{
//	if(dw.format)
//	{
//		IDWriteTextFormat_Release(dw.format);
//		dw.format = NULL;
//	}

//	if(dw.factory)
//	{
//		IDWriteFactory_Release(dw.factory);
//		dw.factory = NULL;
//	}

	return 0;
}

int nf_draw(Tigr * bitmap, const char * text)
{
	HRESULT hr;

	dw.rt->BeginDraw();

	//dw.rt->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
	dw.rt->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	size_t len = strlen(text) + 1;
	WCHAR * wtext = (WCHAR*)alloca(len * sizeof(WCHAR));
	size_t res = std::mbstowcs(wtext, text, len);
	if(res == (size_t)-1)
	{
		printf("failed to convert text to wchar, text : '%s'", text);
		return -1;
	}

	//D2D1_RECT_F rect = {0.0f, 0.0f, (float)bitmap->w, (float)bitmap->h};
	//dw.rt->DrawTextA(wtext, res, dw.format, rect, dw.solid_brush);

	//dw.rt->DrawTextLayout(

	IDWriteTextLayout * layout;
	if(FAILED(hr = dw.factory->CreateTextLayout(wtext, res, dw.format, bitmap->w, bitmap->h, &layout)))
	{
		return -1;
	}

	DWRITE_TEXT_RANGE t;
	t.startPosition = 0;
	t.length = res;
	layout->SetUnderline(true, t);

	dw.rt->DrawTextLayout(D2D1::Point2F(), layout, dw.solid_brush);

	//rt->draw

	dw.rt->EndDraw();
	layout->Release();



	dw.d3d10_device->CopyResource(dw.texture2, dw.texture);


	D3D10_MAPPED_TEXTURE2D mapped;
	if(FAILED(hr = dw.texture2->Map(0, D3D10_MAP_READ, 0, &mapped)))
	{
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		printf("failed map %s\n", errMsg);
		return -1;
	}

	memset(bitmap->pix, 0, bitmap->w * bitmap->h * 4);
	for(size_t j = 0; j < bitmap->h; ++j)
		memcpy((char*)bitmap->pix + j * bitmap->w * 4, (char*)mapped.pData + j * mapped.RowPitch, bitmap->w * 4);

	dw.texture2->Unmap(0);

	return 0;
}
*/

// dynamic linkage stuff
//HMODULE lib;
//	typedef HRESULT (WINAPI * create_fn_t)(
//		_In_ DWRITE_FACTORY_TYPE factoryType,
//		_In_ REFIID iid,
//		_Out_ IUnknown ** factory
//	);
//	create_fn_t create_factory = NULL;
//dw.lib = LoadLibraryA("Dwrite.dll");
//if(!dw.lib)
//	return -1;
//create_factory = (create_fn_t)GetProcAddress(dw.lib, "DWriteCreateFactory");
//if(!create_factory)
//{
//	nf_deinit();
//	return -1;
//}
//if (dw.lib)
//FreeLibrary(dw.lib);
