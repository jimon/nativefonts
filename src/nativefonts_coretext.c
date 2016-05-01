
#include <nativefonts.h>

#ifdef NF_PLATFORM_CORETEXT

#include <CoreText/CoreText.h>
#include <CoreGraphics/CoreGraphics.h>
#include <stdlib.h>
#include <math.h>

nf_system_info_t nf_system_info()
{
	nf_system_info_t ret;
	ret.bitmap = NF_BITMAP_B8G8R8A8_UNORM_PMA; // TODO
	ret.max_width = NF_MAX_WIDTH;
	ret.max_height = NF_MAX_HEIGHT;
	ret.ppi_x = 0.0f; // TODO
	ret.ppi_y = 0.0f;
	return ret;
}

nf_font_t nf_font(const char * font_name, nf_font_params_t params)
{
	CFStringRef font_name_ref = CFStringCreateWithCString(
									NULL, font_name, kCFStringEncodingUTF8);


	CTFontRef font = CTFontCreateWithName(font_name_ref, params.size_in_pt, nil);

	// TODO traits ?

	return font;
}

void nf_free(nf_font_t font)
{
	if(font)
		CFRelease((CTFontRef)font);
}

int nf_print(
	void * bitmap, uint16_t w, uint16_t h,
	nf_font_t font, nf_feature_t * features, size_t features_count,
	nf_aabb_t * result_rect, const char * text, ...)
{
	CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
	CGFloat components[] = { 1.0, 0.0, 0.0, 1.0 };
	CGColorRef red = CGColorCreate(rgbColorSpace, components);
	CGColorSpaceRelease(rgbColorSpace);

	CFStringRef string_ref = CFStringCreateWithCString(NULL, text, kCFStringEncodingUTF8);
	const void * keys[2] = {kCTFontAttributeName, kCTForegroundColorAttributeName};
	const void * values[2] = {font, red};
	CFDictionaryRef dict_ref = CFDictionaryCreate(NULL, keys, values, 2, &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	CFAttributedStringRef text_ref = CFAttributedStringCreate(NULL, string_ref, dict_ref);
	CFRelease(string_ref);
	CFRelease(dict_ref);
	CGColorRelease(red);

	CGMutablePathRef path = CGPathCreateMutable();
	CGRect bounds = CGRectMake(0.0, 0.0, w, h);
	CGPathAddRect(path, NULL, bounds);

	CTFramesetterRef framesetter = CTFramesetterCreateWithAttributedString(text_ref);
	CFRelease(text_ref);
	CTFrameRef frame = CTFramesetterCreateFrame(framesetter, CFRangeMake(0, 0), path, NULL);
	CFRelease(framesetter);
	CFRelease(path);

	//CTLineRef line = CTLineCreateWithAttributedString(text_ref);
	//CGFloat ascent, descent, leading;
	//double fWidth = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
	//CTFrameGet

	CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
	CGContextRef ctx = CGBitmapContextCreate(bitmap, w, h, 8, w * 4, space, kCGImageAlphaPremultipliedLast);
	CGColorSpaceRelease(space);
	//CGContextSetRGBFillColor(ctx, 0.0, 0.0, 0.0, 1.0);
	//CGContextFillRect(ctx, CGRectMake(0.0, 0.0, w, h));
	CGContextClearRect(ctx, CGRectMake(0.0, 0.0, w, h));

	// Draw the text
	//CGFloat x = 0.0;
	//CGFloat y = descent;
	CGContextSetTextPosition(ctx, 0.0, 0.0);
	CTFrameDraw(frame, ctx);

	CFRelease(ctx);
	CFRelease(frame);

	if(result_rect)
	{
		result_rect->x = 0;
		result_rect->y = 0;
		result_rect->w = w;
		result_rect->h = h;
	}


	return 0;
}

#endif

