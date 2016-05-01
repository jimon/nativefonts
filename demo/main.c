// demo of nativefonts

#include "tigr.h"
#include <nativefonts.h>

const char * sample1 =
"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras convallis elementum nunc vitae malesuada. Sed vitae finibus nunc. Mauris ipsum odio, imperdiet id ultricies nec, tristique sed mauris. Integer venenatis lorem id lectus lacinia pulvinar. Maecenas molestie ligula ultrices eros sodales, vel hendrerit elit lacinia. Suspendisse in magna dui. Quisque a arcu nisl.\n"
"\n"
"Phasellus elementum aliquet dolor non convallis. Pellentesque et ligula ante. Quisque eu tristique eros. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. In at ex a metus dapibus hendrerit id id leo. Maecenas sagittis ipsum ac nibh sagittis, vel dictum lectus scelerisque. Phasellus quis lacus in dui placerat hendrerit. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Pellentesque mattis tincidunt risus eget venenatis. Ut pulvinar vehicula neque, et mattis libero molestie ut. Ut quis ornare orci. Ut eu arcu nisl. Vestibulum suscipit iaculis augue sed ultrices.\n"
"\n"
"Donec vitae ex felis. Vestibulum ornare urna non magna imperdiet, et ornare lacus sodales. Pellentesque nibh ligula, dapibus a dapibus nec, gravida at ligula. Vestibulum vitae tellus dignissim elit consectetur tincidunt. In ultricies velit ac efficitur auctor. Cras ut facilisis enim. Integer blandit turpis quis interdum finibus. Phasellus vitae imperdiet odio, sit amet varius nisl. Quisque sollicitudin mauris nisi, eget imperdiet augue dictum in. Nunc aliquet, dolor quis dapibus feugiat, metus mi accumsan nisi, at dictum eros ante nec erat. Phasellus tempor massa non felis rutrum tempus. Fusce ac quam nulla.\n"
"\n"
"Nulla diam metus, cursus eget efficitur consequat, feugiat id est. Integer eu odio sed ex pulvinar vehicula egestas eu dui. Nullam non ante molestie, iaculis dui in, dictum tortor. Phasellus venenatis lacus sed quam dignissim, sed finibus leo ullamcorper. Integer molestie ac sapien vitae ultricies. Mauris a quam nibh. Nam maximus in quam non rutrum. Cras lacinia, lectus eget facilisis semper, enim nisl tempus mi, at malesuada lorem quam nec ante. Duis vestibulum diam libero, dapibus facilisis lorem tempor eu. Duis imperdiet, justo sit amet bibendum ultrices, arcu nisl pretium mauris, in feugiat mauris mi nec dui.\n"
"\n"
"Aliquam ac justo ut eros sagittis accumsan eget semper orci. Vestibulum vehicula quam eget nibh rhoncus pharetra. Donec quis sem rutrum, gravida tortor in, varius turpis. Morbi aliquet ex nec pulvinar ullamcorper. Nam sed cursus ante, ut dapibus turpis. Nunc eu lacus quis felis feugiat lacinia. Nulla placerat id erat et rutrum. Etiam dignissim, odio ut suscipit facilisis, ante purus rutrum tortor, eget ornare nibh lectus sit amet nibh. Sed facilisis orci sed ultricies ultrices. Morbi non imperdiet arcu.\n"
"\n"
"Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Sed posuere venenatis nulla. Pellentesque commodo tellus non tellus lobortis, ut consequat eros scelerisque. Nunc at nunc lorem. Integer justo nisi, ullamcorper varius vulputate quis, dapibus et arcu. Donec sollicitudin elit in magna mattis, a interdum dolor vestibulum. Morbi at ante eros. Nulla et sodales felis. Etiam sagittis libero vitae massa finibus convallis. Integer interdum pretium augue vitae ullamcorper. Aliquam luctus scelerisque lorem vel egestas. Ut tristique turpis sed est ultricies, id commodo velit placerat. Aenean a mi ac odio ornare euismod.\n"
"\n"
"Aenean quis finibus odio, eget dapibus nisl. Maecenas accumsan dolor sed enim tempus vestibulum. Nam semper, nisl eu dapibus sodales, massa metus interdum velit, sed pharetra turpis ipsum in nisl. Cras tincidunt augue quis mi vehicula, sed iaculis magna convallis. Integer sed neque ultricies, mollis magna at, fermentum metus. Curabitur facilisis aliquet ultrices. Integer eget maximus elit. Sed viverra, lorem id ultricies scelerisque, mi nunc finibus ligula, nec porta mi leo at eros. In nec arcu eget sem laoreet ornare. Proin imperdiet turpis erat, ut varius elit molestie et. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Sed lacinia quam vitae volutpat volutpat.\n"
"\n"
"Nunc at quam a sem faucibus pulvinar at eget eros. Sed consequat elit et erat congue commodo. Integer vel dolor suscipit, tincidunt lorem nec, dictum nulla. Nullam suscipit risus tortor, eget sagittis justo tristique id. Mauris mattis tellus vel mi tincidunt porta. Praesent finibus velit ligula, in feugiat sem vulputate quis. Duis massa magna, vulputate quis nisi id, molestie ultricies augue. Quisque ac elementum nisi, eu dapibus arcu. Duis eleifend purus egestas, scelerisque nulla id, eleifend libero. Suspendisse sem odio, facilisis id feugiat sed, dignissim vel elit. Morbi ornare sit amet massa sit amet pharetra. Phasellus at tellus risus. Sed hendrerit, ex volutpat placerat faucibus, eros elit ornare nisi, vitae viverra arcu libero non nulla. Aliquam hendrerit ipsum lacus, quis lobortis nibh rutrum sed. Proin eleifend nisi a tincidunt aliquet.\n"
"\n"
"Interdum et malesuada fames ac ante ipsum primis in faucibus. Sed quis felis imperdiet, auctor risus eu, pharetra sem. Vivamus et feugiat sem. Sed lobortis consectetur euismod. Aliquam id iaculis risus. Curabitur varius, justo nec efficitur venenatis, augue quam tincidunt arcu, ut elementum arcu velit eu nisl. Sed luctus nibh ac turpis posuere sollicitudin.\n"
"\n"
"Quisque mi purus, consequat non massa vitae, efficitur tempor sapien. Nunc nisl ipsum, vehicula venenatis aliquet at, finibus cursus felis. In sollicitudin pretium lorem a tincidunt. Cras imperdiet vel risus a lacinia. Phasellus eget dignissim lacus. Interdum et malesuada fames ac ante ipsum primis in faucibus. Phasellus aliquet, velit in molestie vehicula, mi neque hendrerit diam, ut tristique turpis lorem in augue. Aliquam in orci convallis, efficitur augue et, faucibus ex. Curabitur ullamcorper tellus eu turpis maximus lobortis. Aenean sodales lorem eros, eu consectetur metus faucibus et. Cras faucibus ipsum lorem, sit amet posuere purus mollis varius. Pellentesque porta elementum ex, vel ullamcorper odio porttitor at. Nulla ac vestibulum urna, eget maximus erat. Nam ullamcorper fermentum erat ut convallis. Vivamus id porta erat, vitae elementum est.\n"
"\n"
"Nulla sagittis, orci eget convallis consequat, nisl odio luctus turpis, non sodales enim libero id risus. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Nam maximus eleifend neque, nec viverra neque rutrum quis. Praesent lobortis ex feugiat arcu rutrum imperdiet. Suspendisse potenti. Quisque tempus sagittis fermentum. Quisque id tincidunt nisi. Aliquam massa augue, vulputate non efficitur in, suscipit id est.\n"
"\n"
"Praesent cursus hendrerit arcu eu mattis. Nunc ullamcorper dapibus dignissim. Proin sed faucibus risus. Donec varius sit amet sapien ultricies feugiat. Pellentesque mattis laoreet viverra. Aliquam quis scelerisque eros. Sed vitae mi a diam rutrum condimentum. In finibus quis neque in tristique. Donec nec posuere urna. Phasellus lacinia ornare diam ac pellentesque. Etiam in iaculis nunc. Sed ac tortor pulvinar, tempus eros vel, dictum metus. Sed dui dolor, scelerisque ut tempus venenatis, suscipit sit amet massa.\n"
"\n"
"Maecenas molestie ante ac sapien pretium hendrerit. Quisque et neque ligula. Nulla gravida diam lacus, sed bibendum ipsum gravida convallis. Integer ut diam ut ex mollis porta. Nulla facilisi. Nunc ipsum lacus, mollis sit amet sem at, fermentum egestas enim. Ut ac posuere risus. Integer eu ligula tempus, congue metus ac, tincidunt lorem. Quisque ut augue eget mauris dapibus dictum ornare nec metus. Morbi et fringilla eros, id varius lectus. Nunc eu risus lacus. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos.\n"
"\n"
"Nulla volutpat varius consectetur. Nulla maximus vehicula scelerisque. Curabitur nec lorem nec felis posuere scelerisque sit amet vitae libero. Integer molestie hendrerit dictum. Integer velit sapien, lobortis eget hendrerit vel, viverra eget eros. Vivamus placerat quis magna et venenatis. Aliquam feugiat at urna laoreet ornare. In iaculis erat vitae sapien blandit, ac accumsan ante tempor. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos.\n"
"\n"
"Nullam suscipit aliquet urna eu commodo. Praesent in sem vel odio ultrices bibendum a posuere sem. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Cras ut lorem sagittis arcu congue faucibus non vel tellus. Quisque sagittis, est dapibus suscipit lacinia, leo sem semper sem, quis pretium lacus urna vitae sapien. Aliquam in facilisis orci. Morbi interdum magna posuere rhoncus aliquam. Sed viverra pulvinar tortor, tempus consequat dui tempor vel. Phasellus orci dolor, malesuada ac augue sit amet, interdum consectetur enim. In venenatis elit sit amet est ultrices bibendum. Donec et dictum ipsum. Nunc commodo mattis dui eget malesuada. Nam tincidunt sed nulla in vulputate. Nullam vestibulum libero blandit ipsum euismod eleifend. Phasellus tristique urna sit amet risus condimentum aliquet.\n";

char sample2[256] = {0};

void gen_sample2_str()
{
	for(size_t i = 0; i < sizeof(sample2) - 1; ++i)
		sample2[i] = rand() % ('z' - 'a' + 1) + 'a';
	sample2[sizeof(sample2) - 1] = '\0';
}

int main(int argc, char *argv[])
{
	Tigr * screen = tigrWindow(800, 600, "Native fonts demo", 0);
	Tigr * bitmap1 = tigrBitmap(screen->w, screen->h);
	Tigr * bitmap2 = tigrBitmap(screen->w, screen->h);

	nf_font_params_t params = {0};
	params.size_in_pt = 13.0f;
	nf_font_t font = nf_font("Arial", params);

	nf_feature_t features[2];
	features[0].type = NF_FEATURE_ITALIC;
	features[0].range.start = 6;
	features[0].range.end = 10;
	features[1].type = NF_FEATURE_UNDERLINE;
	features[1].range.start = 12;
	features[1].range.end = 16;

	while(!tigrClosed(screen) && !tigrKeyDown(screen, TK_ESCAPE))
	{
		tigrClear(screen, tigrRGB(0x80, 0x90, 0xa0));

		gen_sample2_str();

		int base_y = 35;

		tigrTime();
		tigrPrint(screen, tfont, 5, base_y, tigrRGB(0xff, 0xff, 0xff), sample2);
		float time1 = tigrTime();

		tigrTime();
		nf_aabb_t aabb1;
		if(nf_print(bitmap1->pix, bitmap1->w, bitmap1->h, font,
					features, 2, &aabb1, sample1) < 0)
			return -1;
		float time2 = tigrTime();

		tigrTime();
		nf_aabb_t aabb2;
		if(nf_print(bitmap2->pix, bitmap2->w, bitmap2->h,
					font, NULL, 0, &aabb2, sample2) < 0)
			return -1;
		float time3 = tigrTime();

		// pma
		tigrBlitTint2(screen, bitmap2, 5, base_y + 20,
					  aabb2.x, aabb2.y, aabb2.w, aabb2.h,
					  tigrRGBA(0xff,0xff,0xff,(unsigned char)(1.0f*255)));
		tigrBlitTint2(screen, bitmap1, 5, base_y + 60,
					  aabb1.x, aabb1.y, aabb1.w, aabb1.h,
					  tigrRGBA(0xff,0xff,0xff,(unsigned char)(1.0f*255)));
		// not pma
		//tigrBlit(screen, bitmap1, 0, 0, 0, 0, bitmap1->w, bitmap1->h);

		tigrPrint(screen, tfont, 5, 5, tigrRGB(0xff, 0xff, 0xff),
			"tigr font took %f ms, long text took %f ms, short text took %f ms",
			time1 * 1000.0f, time2 * 1000.0f, time3 * 1000.0f);

		tigrUpdate(screen);
	}

	nf_free(font);

	tigrFree(bitmap1);
	tigrFree(bitmap2);
	tigrFree(screen);

	return 0;
}
