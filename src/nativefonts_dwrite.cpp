
#include <nativefonts.h>

#define COBJMACROS
#include <initguid.h>
#include <ole2.h>
#include <shobjidl.h>
//#include "dwrite_c.h"

#include <dwrite.h>

#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>

#include <wincodec.h>

#include <stdio.h>

#include <comdef.h>

#include <cstdlib>

#define IDWriteFactory_CreateTextFormat(This,fontFamilyName,fontCollection,fontWeight,fontStyle,fontStretch,fontSize,localeName,textFormat) (This)->CreateTextFormat(fontFamilyName,fontCollection,fontWeight,fontStyle,fontStretch,fontSize,localeName,textFormat)


#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Windowscodecs.lib")

typedef struct
{
	IDWriteFactory * factory;
	IDWriteTextFormat * format;
	ID2D1Factory1 * d2d_factory;
	IWICImagingFactory * wic_factory;

} dwrite_t;

dwrite_t dw;

FLOAT ConvertPointSizeToDIP(FLOAT points)
{
	return (points/72.0f)*96.0f;
}

int nf_init()
{
	HRESULT hr = 0;

	if(FAILED(hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
									   //&IID_IDWriteFactory,
									   __uuidof(IDWriteFactory),
									   (IUnknown**)(&dw.factory))))
	{
		nf_deinit();
		return -1;
	}

//	STDMETHOD(CreateTextFormat)(THIS_
//		WCHAR const *fontFamilyName,
//		IDWriteFontCollection *fontCollection,
//		DWRITE_FONT_WEIGHT fontWeight,
//		DWRITE_FONT_STYLE fontStyle,
//		DWRITE_FONT_STRETCH fontStretch,
//		FLOAT fontSize,
//		WCHAR const *localeName,
//		IDWriteTextFormat **textFormat) PURE;

	if(FAILED(hr = IDWriteFactory_CreateTextFormat(
		dw.factory,
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

	//dw.format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	//dw.format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

	if(FAILED(hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, (void**)&dw.d2d_factory)))
	{
		// todo
		printf("nope1\n");
		return -1;
	}

	if(FAILED(hr = CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		printf("very nope\n");
		return 0;
	}

	if(FAILED(hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&dw.wic_factory
		)))
	{
		printf("nope2\n");
		return -1;
	}

	return 0;
}

int nf_deinit()
{
	/*
	if(dw.format)
	{
		IDWriteTextFormat_Release(dw.format);
		dw.format = NULL;
	}

	if(dw.factory)
	{
		IDWriteFactory_Release(dw.factory);
		dw.factory = NULL;
	}*/

	return 0;
}

int nf_draw(Tigr * bitmap, const char * text)
{

	IWICBitmap * wicbitmap;
	HRESULT hr;

	if(FAILED(hr = dw.wic_factory->CreateBitmapFromMemory(
				  bitmap->w, bitmap->h,
				  GUID_WICPixelFormat32bppPRGBA, bitmap->w * 4,
				  bitmap->w * bitmap->h * 4,
				  (BYTE*)bitmap->pix, &wicbitmap)))
	{
		printf("failed1\n");
		return -1;
	}

	D2D1_RENDER_TARGET_PROPERTIES props;

	props.dpiX = 96.0f;
	props.dpiY = 96.0f;
	props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
	props.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_UNKNOWN;
	props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT; // hardware?
	props.usage = D2D1_RENDER_TARGET_USAGE_NONE;

	ID2D1RenderTarget * rt;

	if(FAILED(hr = dw.d2d_factory->CreateWicBitmapRenderTarget(wicbitmap, &props, &rt)))
	{
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		printf("failed2 %s\n", errMsg);
		return -1;
	}

	ID2D1SolidColorBrush * solid_brush;
	if(FAILED(hr = rt->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &solid_brush)))
	{
		printf("failed to create d2d brush");
		return -1;
	}

	rt->BeginDraw();

	size_t len = strlen(text) + 1;
	WCHAR * wtext = (WCHAR*)alloca(len * sizeof(WCHAR));
	size_t res = std::mbstowcs(wtext, text, len);
	if(res == (size_t)-1)
	{
		printf("failed to convert text to wchar, text : '%s'", text);
		return -1;
	}

	D2D1_RECT_F rect = {0, 0, bitmap->w, bitmap->h};
	rt->DrawTextA(wtext, len - 1, dw.format, rect, solid_brush);

	rt->EndDraw();


	solid_brush->Release();

	rt->Release();

	WICRect rcLock = {0, 0, bitmap->w, bitmap->h};
	IWICBitmapLock *pILock = NULL;

	if(FAILED(hr = wicbitmap->Lock(&rcLock, WICBitmapLockWrite, &pILock)))
	{
		printf("failed3\n");
		return -1;
	}

	UINT cbBufferSize = 0;
	BYTE *pv = NULL;

	if(FAILED(hr = pILock->GetDataPointer(&cbBufferSize, &pv)))
	{
		printf("failed4\n");
		return -1;
	}

	//printf("wut %i\n", cbBufferSize);

	memcpy(bitmap->pix, pv, cbBufferSize);

	pILock->Release();

	wicbitmap->Release();


//	STDMETHOD(CreateTextLayout)(THIS_
//		WCHAR const *string,
//		UINT32 stringLength,
//		IDWriteTextFormat *textFormat,
//		FLOAT maxWidth,
//		FLOAT maxHeight,
//		IDWriteTextLayout **textLayout) PURE;

	// Create a text_layout, a high-level text rendering facility, using
	// the given codepoint and dummy format.

	/*IDWriteTextLayout * text_layout;

	HRESULT hr = IDWriteFactory_CreateTextLayout(dw.factory, L"test", 4, dw.format, 0.0f, 0.0f, &text_layout);
	if(FAILED(hr)) {
		return -1;
	}

	// Draw the layout with a dummy renderer, which logs the
	// font used and stores it.
	IDWriteFont *font = NULL;
	hr = IDWriteTextLayout_Draw(text_layout, &font, &renderer.iface, 0.0f, 0.0f);
	if (FAILED(hr) || font == NULL) {
		IDWriteTextLayout_Release(text_layout);
	}

	// We're done with these now
	IDWriteTextLayout_Release(text_layout);*/
	return 0;
}


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
