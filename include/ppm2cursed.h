#ifndef PPM2CURSED_H
#define PPM2CURSED_H

#include <stdint.h>

typedef struct rgb{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RGB;

typedef struct image{
	RGB* pixels;
	int height;
	int width;
	int errorOccured;
}Image;


#endif // PPM2CURSED_H
