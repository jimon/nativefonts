#include "tigr.h"

#include <nativefonts.h>

int main(int argc, char *argv[])
{

	int state = nf_init();

	printf("state %i\n", state);

	if(state < 0)
		return -1;

	Tigr *screen = tigrWindow(320, 240, "Hello", 0);

	Tigr *bitmap = tigrBitmap(200, 100);

	while (!tigrClosed(screen) && !tigrKeyDown(screen, TK_ESCAPE))
	{
		tigrClear(screen, tigrRGB(0x80, 0x90, 0xa0));
		tigrPrint(screen, tfont, 120, 110, tigrRGB(0xff, 0xff, 0xff), "Hello, world.");

		if(nf_draw(bitmap, "Why, hello there!") < 0)
			return -1;
		tigrBlitAlpha(screen, bitmap, 0, 0, 0, 0, bitmap->w, bitmap->h, 1.0f);


		tigrUpdate(screen);
	}

	tigrFree(bitmap);
	tigrFree(screen);

	nf_deinit();
	return 0;
}
