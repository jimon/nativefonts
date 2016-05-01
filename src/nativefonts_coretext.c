
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

// based on http://stackoverflow.com/questions/8377496/how-to-get-the-real-height-of-text-drawn-on-a-ctframe
CGSize nf_frame_size(CTFrameRef frame)
{
	CFArrayRef lines = CTFrameGetLines(frame);
	CFIndex numLines = CFArrayGetCount(lines);
	CGFloat maxWidth = 0.0f;

	for(CFIndex index = 0; index < numLines; index++)
	{
		CTLineRef line = (CTLineRef)CFArrayGetValueAtIndex(lines, index);
		CGFloat width = CTLineGetTypographicBounds(line, NULL, NULL, NULL);

		if(width > maxWidth)
			maxWidth = width;
	}

	CGFloat ascent = 0.0f, descent = 0.0f, leading = 0.0f;
	CTLineGetTypographicBounds((CTLineRef)CFArrayGetValueAtIndex(lines, 0), &ascent,  &descent, &leading);
	CGFloat firstLineHeight = ascent + descent + leading;

	CTLineGetTypographicBounds((CTLineRef)CFArrayGetValueAtIndex(lines, numLines - 1), &ascent,  &descent, &leading);
	CGFloat lastLineHeight  = ascent + descent + leading;

	CGPoint firstLineOrigin;
	CTFrameGetLineOrigins(frame, CFRangeMake(0, 1), &firstLineOrigin);

	CGPoint lastLineOrigin;
	CTFrameGetLineOrigins(frame, CFRangeMake(numLines - 1, 1), &lastLineOrigin);

	CGFloat textHeight = abs(firstLineOrigin.y - lastLineOrigin.y) + firstLineHeight + lastLineHeight;

	return CGSizeMake(maxWidth, textHeight);
}

int nf_print(
	void * bitmap, uint16_t w, uint16_t h,
	nf_font_t font, nf_feature_t * features, size_t features_count,
	nf_aabb_t * result_rect, const char * text, ...)
{
	CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
	CGFloat components[] = { 1.0, 1.0, 1.0, 1.0 };
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
	CGContextRef ctx = CGBitmapContextCreate(bitmap, w, h, 8, w * 4, space, kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little);
	CGColorSpaceRelease(space);
	//CGContextSetRGBFillColor(ctx, 0.0, 0.0, 0.0, 1.0);
	//CGContextFillRect(ctx, CGRectMake(0.0, 0.0, w, h));
	CGContextClearRect(ctx, CGRectMake(0.0, 0.0, w, h));

	// Draw the text
	CGContextSetTextPosition(ctx, 0.0, 0.0);
	CTFrameDraw(frame, ctx);

	CFRelease(ctx);

	if(result_rect)
	{
		CGSize size = nf_frame_size(frame);
		result_rect->x = 0;
		result_rect->y = 0;
		result_rect->w = size.width;
		result_rect->h = size.height;
	}

	CFRelease(frame);
	return 0;
}

#endif

