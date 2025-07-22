#include "cursed_display.h"
#include <stdio.h>
#define DISPLAY_HEIGHT 20
#define DISPLAY_WIDTH 20

#define DEBUG

int main(){
	CursedDisplay *display = createCursedDisplay((CursedDisplaySettings){DISPLAY_HEIGHT, DISPLAY_WIDTH});
	if (display==NULL){
		printf("CursedDisplay returned null.\n");
	}
	printf("Successfully Created cursed display @%p\n\r", display);
	for (int i = 0; i<DISPLAY_HEIGHT; i++){
		setPixel(display, i,i, CDCOLOR_BLUE);
	}
	printDisplay(display);
	writeToDebugWindow(display,0, "DEBUG WINDOW:");
	refreshDisplay(display, 0);
	waitForInput(display);
	destroyCursedDisplay(display);
	printf("Destroyed cursed display. \n\r");
}
