#ifndef CURSED_DISPLAY_INTERNAL_H
#define CURSED_DISPLAY_INTERNAL_H

#include <ncurses.h>
#include "cursed_display.h"

struct cursed_display{
	CursedDisplaySettings settings;
	int chRows; // term rows
	int chCols; // term cols
	CDCOLOR **frameBuffer;
};

#endif // CURSED_DISPLAY_INTERNAL_H
