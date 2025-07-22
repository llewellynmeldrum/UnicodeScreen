#ifndef CURSED_DISPLAY_H
#define CURSED_DISPLAY_H

/* FOR PUBLIC */
typedef enum{
	CDCOLOR_BLACK   = 0,
        CDCOLOR_RED     = 1,
        CDCOLOR_GREEN   = 2,
        CDCOLOR_YELLOW  = 3,
        CDCOLOR_BLUE    = 4,
        CDCOLOR_MAGENTA = 5,
        CDCOLOR_CYAN    = 6,
        CDCOLOR_WHITE   = 7,
} CDCOLOR;


typedef struct cursed_display CursedDisplay;

typedef struct cursed_display_settings{
	int pxHeight; 	
	int pxWidth;
} CursedDisplaySettings;


/* ---------------------------------------------- */
/* -------- PUBLIC INTERFACING FUNCTIONS -------- */
/* ---------------------------------------------- */

CursedDisplay *createCursedDisplay(CursedDisplaySettings settings);
void destroyCursedDisplay(CursedDisplay *display);

CDCOLOR getPixel(CursedDisplay *, int py, int px);
void setPixel(CursedDisplay *, int py, int px, CDCOLOR col);

void refreshDisplay(CursedDisplay *, float minRefreshTime);

void printDisplay(CursedDisplay *);

void writeToDebugWindow(CursedDisplay *display, int line, const char *fmt, ...);
void waitForInput(CursedDisplay *);

#endif // CURSED_DISPLAY_H
