#include "cursed_display.h"
#include <stdio.h>
#define DISPLAY_HEIGHT 40
#define DISPLAY_WIDTH 40

#define DEBUG

int main(){
	CursedDisplay *display = createCursedDisplay((CursedDisplaySettings){DISPLAY_HEIGHT, DISPLAY_WIDTH});
	writeToDebugWindow(display,0, "DEBUG WINDOW:");
	refreshDisplay(display, 0);
	waitForInput(display);
	destroyCursedDisplay(display);
	debug_print_log();
	printf("Destroyed cursed display. \n\r");
}
