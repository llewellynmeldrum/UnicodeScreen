#ifndef CURSED_DISPLAY_INTERNAL_H
#define CURSED_DISPLAY_INTERNAL_H

#include <ncurses.h>
#include "cursed_display.h"

#define DEBUG_WIN_ROWS 3

struct cursed_display{
	CursedDisplaySettings settings;
	int chRows; // term rows
	int chCols; // term cols
	CDCOLOR **frameBuffer;
	WINDOW* displayWindow;
	WINDOW* debugWindow;
};


void init_ncurses(CursedDisplay *display);
WINDOW *createDisplayWindow(CursedDisplay *display);
WINDOW *createDebugWindow(CursedDisplay *display); // debug window should sit below and be 
void end_ncurses();

void dump_display_window(CursedDisplay *display);
#endif // CURSED_DISPLAY_INTERNAL_H
