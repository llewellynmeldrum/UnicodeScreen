#ifndef UNICODE_SCREEN_INTERNAL_H
#define UNICODE_SCREEN_INTERNAL_H

#include <ncurses.h>
#include "unicode_screen.h"

#define DEBUG_WIN_ROWS 3

struct unicode_screen{
	UnicodeScreenSettings settings;
	int chRows; // term rows
	int chCols; // term cols
	CDCOLOR **frameBuffer;
	WINDOW* displayWindow;
	WINDOW* debugWindow;
};


void init_ncurses(UnicodeScreen *display);
WINDOW *createDisplayWindow(UnicodeScreen *display);
WINDOW *createDebugWindow(UnicodeScreen *display); // debug window should sit below and be 
void end_ncurses();

void dump_display_window(UnicodeScreen *display);
#endif // UNICODE_SCREEN_INTERNAL_H
