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
#define IMG_ERROR -1
#define USAGE_STR "<path/to/image>"


#define println(fmt, ...) printf(fmt "\r\n", ##__VA_ARGS__)


RGB* getPixels_PPM(FILE* fptr, int *height_p, int* width_p);
Image openPPM(const char* path);
Image openImage(const char* path);
int indexIntoFlatCM(int y, int x, int width);
int pushImageToDisplay(UnicodeScreen *display, Image image);
CDCOLOR getClosestCDCOLOR(RGB pixel);
void INITCDCOLOR_RGBVAL();

char CDCOLOR_str[N_CDCOLORS][7] = {
        "BLACK",
        "RED",
        "GREEN",
        "YELLOW",
        "BLUE",
        "PURPLE",
        "CYAN",
        "WHITE",
};

void printCDCOLOR(CDCOLOR col){
	if (col<0 || col>CDCOLOR_MAX){
		printf("[INVALID_CDCOLOR]");
	} else {
		printf("%s",CDCOLOR_str[col]);
	}
}

void printRGB(RGB col){
	printf("[%hhu,%hhu,%hhu]",col.r,col.g,col.b);
}

int main(int argc, char** argv){
	INITCDCOLOR_RGBVAL();
	if (argc!=2){
		fprintf(stderr, "Incorrect usage. Try: %s %s\n", argv[0], USAGE_STR);
		exit(EXIT_FAILURE);
	}
	char* image_path = argv[1];
	Image image = openImage(image_path); 

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
	pushImageToDisplay(display, image);
	refreshDisplay(display, 0);
	waitForInput(display);
	destroyCursedDisplay(display);
	debug_print_log();
	printf("Destroyed cursed display. \n\r");
}



Image openImage(const char* path){
	// if im able to discern some stuff from users terminal i can make this a custom value
	const int max_img_width = 128;
	const int max_img_height = 64;

	// Open image, set fields...
	
	Image image;
	if (image.width>max_img_width){
		// Resize image such that 
		//	newWidth = max_img_width
		//	newHeight = (newWidth/aspectRatio.x)*aspectRatio.y
	}

	if (image.height>max_img_height){
		// Resize image such that
		//	newHeight = max_img_height
		//	newWidth = (newHeight/aspectRatio.y)*aspectRatio.x

	}

	if (image.height%2!=0){
		image.height += 1;

		// Set bottom row of pixels to black...

	}


	return (Image){};
}


int pushImageToDisplay(UnicodeScreen *display, Image image){
	int displayHeight = getDisplayHeight(display);
	int displayWidth = getDisplayWidth(display);
	if (displayHeight != image.height || displayWidth != image.width){
		println("WARNING!!! DISPLAY DIMENSIONS DO NOT MATCH IMAGE DIMENSIONS!!!");
	}
	for (int y = 0; y<displayHeight; y++){
		for (int x = 0; x<displayWidth; x++){
			int i = indexIntoFlatCM(y, x, displayWidth); 
			CDCOLOR col = getClosestCDCOLOR(image.pixelsCM[i]);
			setPixel(display, x,y, col);
		}
	}
	return 0; 
}

CDCOLOR getClosestCDCOLOR(RGB pixel){
	// the nearest color has the lowest summary difference in colors.
	// i.e rDiff + gDiff + bDiff < all the others.
	uint32_t min_diffsum = RGB_MAX;
	int nearestcol_idx = 0;
	for (int i = 0; i<N_CDCOLORS; i++){
		RGB col = CDCOLOR_RGBVAL[i];
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


Image openPPM(const char* path){
	FILE *fptr = fopen(path, "rb");
	int height, width;
	int err = 0;
	RGB* pixels;
	if (fptr==NULL){
		fprintf(stderr, "\n\rError, file header is corrupted or image is not PPM format. Exiting.\n\r");
		err = IMG_ERROR;
	} else {
		pixels = getPixels_PPM(fptr, &height, &width); 
		if (pixels==NULL){
			fprintf(stderr, "\n\rError, file header is corrupted or image is not PPM format. Exiting.\n\r");
			err = IMG_ERROR;
		}
	}
	Image image = (Image){
		.pixelsCM = pixels, 
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

RGB* getPixels_PPM(FILE* fptr, int *height_p, int* width_p){ 
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

	RGB* RGB_grid = malloc(sizeof(RGB) * height * width);

	for (int y = 0; y<height; y++){
		for (int x = 0; x<width; x++){
			int i = (y*width)+x;
			uint8_t r,g,b;
			//fskipwhitespace(fptr);
			fread(&r, byte_size, 1, fptr);
			fread(&g, byte_size, 1, fptr);
			fread(&b, byte_size, 1, fptr);
			RGB_grid[i] = (RGB){ r,g,b };
	//		println("[%03d]: %02X %02X %02X", i, r,g,b);
		}
	}
	return RGB_grid;
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
int indexIntoFlatCM(int y, int x, int width){
	return (x*width) + y;
}
void INITCDCOLOR_RGBVAL(){
	CDCOLOR_RGBVAL[CDCOLOR_BLACK]	=	(RGB){0,   0,   0  };
	CDCOLOR_RGBVAL[CDCOLOR_RED]	=	(RGB){153, 0,   0  };
	CDCOLOR_RGBVAL[CDCOLOR_GREEN]	=	(RGB){0,   166, 0  };
	CDCOLOR_RGBVAL[CDCOLOR_YELLOW]	=	(RGB){153, 153, 0  };
	CDCOLOR_RGBVAL[CDCOLOR_BLUE]	=	(RGB){0,   0,   178};
	CDCOLOR_RGBVAL[CDCOLOR_PURPLE]	=	(RGB){178, 0,   178};
	CDCOLOR_RGBVAL[CDCOLOR_CYAN]	=	(RGB){0,   166, 178};
	CDCOLOR_RGBVAL[CDCOLOR_WHITE]	=	(RGB){191, 191, 191};
}
