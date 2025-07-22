#include "cursed_display.h"
#include <stdio.h>
#define DISPLAY_HEIGHT 10
#define DISPLAY_WIDTH 10

#define DEBUG

int main(){
	CursedDisplay *display = createCursedDisplay((CursedDisplaySettings){DISPLAY_HEIGHT, DISPLAY_WIDTH});
	if (display==NULL){
		printf("CursedDisplay returned null.\n");
	}
	printf("Successfully Created cursed display @%p\n\r", display);
	for (int y = 0; y<2; y++){
		for (int x = 0; x<DISPLAY_WIDTH; x++){
			setPixel(display, y, x, x);
		}
	}
	printDisplay(display);
	writeToDebugWindow(display,0, "DEBUG WINDOW:");
	refreshDisplay(display, 0);
	waitForInput(display);
	destroyCursedDisplay(display);
	printf("Destroyed cursed display. \n\r");
}
