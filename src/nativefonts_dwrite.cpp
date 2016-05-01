
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

//#include <wincodec.h>

#include <stdio.h>

#include <comdef.h>

#include <cstdlib>

#define IDWriteFactory_CreateTextFormat(This,fontFamilyName,fontCollection,fontWeight,fontStyle,fontStretch,fontSize,localeName,textFormat) (This)->CreateTextFormat(fontFamilyName,fontCollection,fontWeight,fontStyle,fontStretch,fontSize,localeName,textFormat)


#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "D3D10_1.lib")

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
/*
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
*/

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

	/*
	//memset((BYTE*)bitmap->pix, 0, bitmap->w * bitmap->h * 4);

//	if(FAILED(hr = dw.wic_factory->CreateBitmapFromMemory(
//				  bitmap->w, bitmap->h,
//				  GUID_WICPixelFormat32bppPRGBA, bitmap->w * 4,
//				  bitmap->w * bitmap->h * 4,
//				  (BYTE*)bitmap->pix, &wicbitmap)))

	if(FAILED(hr = dw.wic_factory->CreateBitmap(
				  bitmap->w, bitmap->h,
				  GUID_WICPixelFormat32bppPBGRA,
				  WICBitmapCacheOnDemand,
				  &wicbitmap)))
	{
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		printf("failed1 %s\n", errMsg);
		return -1;
	}


	float dpi_x, dpi_y;
	dw.d2d_factory->GetDesktopDpi(&dpi_x, &dpi_y);

	D2D1_RENDER_TARGET_PROPERTIES props;

	props.dpiX = dpi_x;
	props.dpiY = dpi_y;
	props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
	props.pixelFormat.format = DXGI_FORMAT_UNKNOWN; //DXGI_FORMAT_B8G8R8A8_UNORM;
	props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_UNKNOWN ;//D2D1_ALPHA_MODE_PREMULTIPLIED;
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
	*/


	//params->
	//rt->SetTextRenderingParams(params);
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

	//dw.d3d10_device->CopyResource(dw.texture, dw.texture2);


	/*
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

	// undo pma (facepalm)
//	for(size_t j = 0; j < bitmap->h; ++j)
//	{
//		for(size_t i = 0; i < bitmap->w; ++i)
//		{
//			TPixel & pixel = bitmap->pix[j * bitmap->w + i];
//			float a = pixel.a / 255.0f;
//			pixel.r /= a;
//			pixel.g /= a;
//			pixel.b /= a;
//		}
//	}

	pILock->Release();

	wicbitmap->Release();*/


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
