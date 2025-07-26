#ifndef IMG2UNISCR_H
#define IMG2UNISCR_H

#include <stdint.h>

typedef struct aspect_ratio{
	int x;
	int y;
}AspectRatio ;

typedef struct rgb{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RGB;

typedef struct image{
	RGB* pixelsCM;
	int height;
	int width;
	AspectRatio aspectRatio;
	int errorOccured;
} Image;


#endif // IMG2UNISCR_H
