#ifndef UNICODE_SCREEN_H
#define UNICODE_SCREEN_H

#include "img2uniscr.h"
/* FOR PUBLIC */
typedef enum{
        CDCOLOR_BLACK   = 0, 
        CDCOLOR_RED     = 1,
        CDCOLOR_GREEN   = 2,
        CDCOLOR_YELLOW  = 3,
        CDCOLOR_BLUE    = 4,
        CDCOLOR_PURPLE  = 5,
	CDCOLOR_CYAN    = 6,
        CDCOLOR_WHITE   = 7,
} CDCOLOR;


#define N_CDCOLORS 8
#define CDCOLOR_MAX 7 
#define RGB24_MAX 255 

RGB24 CDCOLOR_RGBVAL[N_CDCOLORS];



typedef struct unicode_screen UnicodeScreen;

typedef struct unicode_screen_settings{
	int pxHeight; 	
	int pxWidth;
} UnicodeScreenSettings;


void debug_log_all();
void debug_print_log();
/* ---------------------------------------------- */
/* -------- PUBLIC INTERFACING FUNCTIONS -------- */
/* ---------------------------------------------- */

UnicodeScreen *createCursedDisplay(UnicodeScreenSettings settings);
void destroyCursedDisplay(UnicodeScreen *display);

CDCOLOR getPixel(UnicodeScreen *, int py, int px);
void setPixel(UnicodeScreen *, int py, int px, CDCOLOR col);

void refreshDisplay(UnicodeScreen *, float minRefreshTime);

int getDisplayHeight(UnicodeScreen * d);
int getDisplayWidth(UnicodeScreen * d);
void printDisplay(UnicodeScreen *);

void writeToDebugWindow(UnicodeScreen *display, int line, const char *fmt, ...);
void waitForInput(UnicodeScreen *);

#endif // UNICODE_SCREEN_H
