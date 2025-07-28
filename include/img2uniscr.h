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
} RGB24;

typedef struct image{
	RGB24* pixelsRM; // row major, flat packing
	int height;
	int width;
	AspectRatio aspectRatio;
	int errorOccured;
} RGB24Image;


#endif // IMG2UNISCR_H
