#include "unicode_screen.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>

// external libraries
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"


#define DEBUG
#define RET_RGB24_IMG_ERR (RGB24Image){.errorOccured = true};
#define USAGE_STR "<path/to/image>"

RGB24* getPixels_PPM(FILE* fptr, int *height_p, int* width_p);	// deprecated
RGB24Image openPPM(const char* path);				// deprecated
// TODO: delete when generic openImage is implemented.


static RGB24Image openImage(const char* path);
static int indexIntoFlatRM(int y, int x, int width);
static int pushImageToDisplay(RGB24Image image, UnicodeScreen *display);

static CDCOLOR getClosestCDCOLOR(RGB24 pixel);
static void INITCDCOLOR_RGBVAL();
static void printCDCOLOR(CDCOLOR col);
static void printRGB24(RGB24 col);
static void printRGB24_HEX(RGB24 col);

static int indexIntoFlatRM_3CHANNEL(int y, int x, int width);
char CDCOLOR_str[N_CDCOLORS][7] = { "BLACK", "RED", "GREEN", "YELLOW", "BLUE", "PURPLE", "CYAN", "WHITE",};

#define println(fmt, ...) printf(fmt "\r\n", ##__VA_ARGS__)



int main(int argc, char** argv){
	INITCDCOLOR_RGBVAL();
	if (argc!=2){
		fprintf(stderr, "Incorrect usage. Try: %s %s\n", argv[0], USAGE_STR);
		exit(EXIT_FAILURE);
	}
	char* image_path = argv[1];
	RGB24Image image = openImage(image_path); 

	if (image.errorOccured){
		fprintf(stderr, "Failed to open file <%s>.\n", image_path); 
		exit(EXIT_FAILURE);
	}

	UnicodeScreenSettings displaySettings = (UnicodeScreenSettings){
		.pxHeight = image.height,
		.pxWidth = image.width,
	};


	UnicodeScreen *display = createCursedDisplay(displaySettings);
	if (display==NULL){
		fprintf(stderr, "Exiting.\n"); 
		exit(EXIT_FAILURE);
	}

	pushImageToDisplay(image, display);
	refreshDisplay(display, 0);
	waitForInput(display);
	destroyCursedDisplay(display);
	debug_print_log();
	printf("Destroyed cursed display. \n\r");
}


/* credit my brother Euclid o7 */
int calc_gcd(int a, int b){
	int t;
	while (b!=0){
		t = b;
		b = a%b;
		a = t;
	}
	return a;
}

AspectRatio calcAspectRatio(int w, int h){
	int gcd = calc_gcd(w,h);
	int x = w/gcd;
	int y = h/gcd;
	return (AspectRatio){.x = x, .y = y};
}

RGB24Image openImage(const char* path){
	// if im able to discern some stuff from users terminal i can make this a custom value
	const int max_img_width = 96;
	const int max_img_height = 96;

	typedef unsigned char* stb_image_t;

	int in_width, in_height, channels;
	stb_image_t in_stb_img = stbi_load(path, &in_width, &in_height, &channels, 3);
	if (in_stb_img == NULL 
		|| in_width > max_img_width || in_height > max_img_height){
		// TODO: REMOVE ABOVE CONDITION WHEN RESIZING IS FIXED 
		return RET_RGB24_IMG_ERR; 
	} 
	println("%s loaded, w=%dpx, h=%dpx, num_channels=%d", path, in_width, in_height, channels);
	
	RGB24Image image = (RGB24Image){
		.height = in_height,
		.width = in_width, 
		.aspectRatio = calcAspectRatio(in_width, in_height),
		.pixelsRM = NULL, 
		.errorOccured = false,
	};

	int new_width = image.width;
	int new_height = image.height;

	if (image.width>max_img_width){
		new_width = max_img_width; 
		new_height = (new_width/image.aspectRatio.x)*image.aspectRatio.y;
	} 

	if (image.height>max_img_height){
		new_height = max_img_height; 
		new_width = (new_height/image.aspectRatio.y)*image.aspectRatio.x;
	}

	println("%s resizing to, w=%dpx, h=%dpx", path, new_width, new_height);

	stb_image_t size_fixed_stb_img = (stb_image_t)stbir_resize_uint8_srgb(
		in_stb_img, in_width, in_height, 0,
		NULL, new_width, new_height, 0, STBIR_BGR
	);

	bool oddheightfixed;
	if (image.height%2!=0){
		image.height += 1;
		oddheightfixed = true;
	}
	image.pixelsRM = malloc(sizeof(RGB24) * image.height * image.width);

	for (int y = 0; y<new_height; y++){
		for (int x = 0; x<new_width; x++){
			int i = indexIntoFlatRM_3CHANNEL(y, x, new_width);
			int i_nochan = indexIntoFlatRM(y, x, new_width);
			unsigned char r = size_fixed_stb_img[i]; 
			unsigned char g = size_fixed_stb_img[i+1]; 
			unsigned char b = size_fixed_stb_img[i+2]; 
			image.pixelsRM[i_nochan] = (RGB24){r, g, b};
		}
	}

	free(size_fixed_stb_img);
	free(in_stb_img);

	if (oddheightfixed){
		int y = new_height;
		for (int x = 0; x<new_width; x++){
			int i = indexIntoFlatRM(y,x,image.width);
			image.pixelsRM[i] = (RGB24){0,0,0};
		}
	}

	return image;
}


int pushImageToDisplay(RGB24Image image, UnicodeScreen *display){
	int displayHeight = getDisplayHeight(display);
	int displayWidth = getDisplayWidth(display);
	for (int y = 0; y<displayHeight; y++){
		for (int x = 0; x<displayWidth; x++){
			int i = indexIntoFlatRM(y, x, displayHeight); 
			CDCOLOR col = getClosestCDCOLOR(image.pixelsRM[i]);
			setPixel(display, y,x, col);
		}
	}
	return 0; 
}

CDCOLOR getClosestCDCOLOR(RGB24 pixel){
	// the nearest color has the lowest summary difference in colors.
	// i.e rDiff + gDiff + bDiff < all the others.
	uint32_t min_diffsum = RGB24_MAX;
	int nearestcol_idx = 0;
	for (int i = 0; i<N_CDCOLORS; i++){
		RGB24 col = CDCOLOR_RGBVAL[i];
		uint32_t rDiff = abs(pixel.r - col.r);
		uint32_t gDiff = abs(pixel.g - col.g);
		uint32_t bDiff = abs(pixel.b - col.b);
		uint32_t diffsum = rDiff+gDiff+bDiff;
		if (diffsum<min_diffsum){
			min_diffsum = diffsum;
			nearestcol_idx = i;
		}
	}
	return nearestcol_idx;
}

// deprecated 
RGB24Image openPPM(const char* path){
	FILE *fptr = fopen(path, "rb");
	int height, width;
	int err = 0;
	RGB24* pixels;
	if (fptr==NULL){
		fprintf(stderr, "\n\rError, file header is corrupted or image is not PPM format. Exiting.\n\r");
		err = true;
	} else {
		pixels = getPixels_PPM(fptr, &height, &width); 
		if (pixels==NULL){
			fprintf(stderr, "\n\rError, file header is corrupted or image is not PPM format. Exiting.\n\r");
			err = true;
		}
	}
	RGB24Image image = (RGB24Image){
		.pixelsRM = pixels, 
		.height = height, 
		.width  =  width, 
		.errorOccured = err,
	};
	fclose(fptr);
	return image;
}

int fscan_StringToInt(FILE* fp){
	char temp[10]; 
	fscanf(fp, " %s ", temp);
	return atoi(temp); 
}

// deprecated 
RGB24* getPixels_PPM(FILE* fptr, int *height_p, int* width_p){ 
//	printf("\n\r"); // fixes broken carriage return shit
	char P, six; 
	fscanf(fptr, "%c%c", &P, &six);
	if (P!='P' || six!='6'){
		return NULL;
	}

	int width = fscan_StringToInt(fptr);
	*width_p = width;
		
	int height = fscan_StringToInt(fptr);
	*height_p = height;

	
	int max_color_val = fscan_StringToInt(fptr);

	size_t byte_size;
	if (max_color_val<256){
		byte_size = 1;
	} else {
		byte_size = 2;
	}
	//fseek(fptr, 1, SEEK_CUR);

	RGB24* RGB24_grid = malloc(sizeof(RGB24) * height * width);

	for (int y = 0; y<height; y++){
		for (int x = 0; x<width; x++){
			int i = (y*width)+x;
			uint8_t r,g,b;
			//fskipwhitespace(fptr);
			fread(&r, byte_size, 1, fptr);
			fread(&g, byte_size, 1, fptr);
			fread(&b, byte_size, 1, fptr);
			RGB24_grid[i] = (RGB24){ r,g,b };
	//		println("[%03d]: %02X %02X %02X", i, r,g,b);
		}
	}
	return RGB24_grid;
}


static void fskipwhitespace(FILE *fp){
	int ch;
	while(feof(fp)==false){
		ch = fgetc(fp);
		if (isspace(ch) == false){
			return;
		}
		println("Skipped ws");
	}
	println("FPOS AFTER: %ld", ftell(fp));
}

static int indexIntoFlatRM(int y, int x, int width){
	return (y*width) + x;
}
static int indexIntoFlatRM_3CHANNEL(int y, int x, int width){
	int c = 3;
	return ((y*width) + x)*c;
}


void INITCDCOLOR_RGBVAL(){
	CDCOLOR_RGBVAL[CDCOLOR_BLACK]	=	(RGB24){0,   0,   0  };
	CDCOLOR_RGBVAL[CDCOLOR_RED]	=	(RGB24){153, 0,   0  };
	CDCOLOR_RGBVAL[CDCOLOR_GREEN]	=	(RGB24){0,   166, 0  };
	CDCOLOR_RGBVAL[CDCOLOR_YELLOW]	=	(RGB24){153, 153, 0  };
	CDCOLOR_RGBVAL[CDCOLOR_BLUE]	=	(RGB24){0,   0,   178};
	CDCOLOR_RGBVAL[CDCOLOR_PURPLE]	=	(RGB24){178, 0,   178};
	CDCOLOR_RGBVAL[CDCOLOR_CYAN]	=	(RGB24){0,   166, 178};
	CDCOLOR_RGBVAL[CDCOLOR_WHITE]	=	(RGB24){191, 191, 191};
}

void printCDCOLOR(CDCOLOR col){
	if (col<0 || col>CDCOLOR_MAX){
		printf("[INVALID_CDCOLOR]");
	} else {
		printf("%s",CDCOLOR_str[col]);
	}
}

void printRGB24(RGB24 col){
	printf("[%hhu,%hhu,%hhu]",col.r,col.g,col.b);
}
void printRGB24_HEX(RGB24 col){
	printf("%02hhx%02hhx%02hhx ",col.r,col.g,col.b);
}
