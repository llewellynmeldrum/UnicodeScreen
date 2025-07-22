#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "cursed_display_internal.h"

// convinience macros
#define BEGIN_CURSES()	initscr(); 
#define END_CURSES()	endwin(); 

/* ---------------------------------------------- */
/* ------------- PRIVATE FUNCTIONS -------------- */
/* ---------------------------------------------- */

void init_ncurses(){
	BEGIN_CURSES(); // begin curses mode
	noecho(); // dont care about input for now 
}

WINDOW *createDisplayWindow(CursedDisplay *display){
	const int startX = 0;
	const int startY = 0;
	WINDOW * displayWindow = newwin(display->chRows, display->chCols, startY, startX);
	return displayWindow;
}

WINDOW *createDebugWindow(CursedDisplay *display){
	const int startY = display->chRows;
	const int startX = 0;

	const int DEBUG_WIN_COLS = display->chCols;
	WINDOW * debugWindow = newwin(DEBUG_WIN_ROWS, DEBUG_WIN_COLS, startY, startX);
	return debugWindow;
}

void end_ncurses(){
	END_CURSES();
}
/* ------------------------------------------------------ */
/* ------------- PRIVATE HELPER FUNCTIONS --------------- */
/* ------------------------------------------------------ */


/* ---------------------------------------------- */
/* -------- PUBLIC INTERFACING FUNCTIONS -------- */
/* ---------------------------------------------- */

CursedDisplay *createCursedDisplay(CursedDisplaySettings settings){
	if (settings.pxHeight<=0 || settings.pxHeight%2!=0 || settings.pxWidth<=0){
		fprintf(stderr, "Error creating display, bad input for display settings. Returning null.\n");
		return NULL;
	}

	CursedDisplay *display = malloc(sizeof(CursedDisplay));
	if (display==NULL){
		fprintf(stderr, "Error allocating space for display. Returning null.\n");
		return NULL;
	}
	display->settings = settings;
	display->chRows = settings.pxHeight / 2;
	display->chCols = settings.pxWidth;


	display->frameBuffer = malloc(sizeof(CDCOLOR *) * display->settings.pxHeight);
	printf("Allocated space for %d rows.\n", display->settings.pxHeight);
	if (display->frameBuffer==NULL){
		fprintf(stderr, "Error allocating space for display framebuf. Returning null.\n");
		free(display);
		return NULL; 
	}

	for (int y = 0; y<display->settings.pxHeight; y++){
		display->frameBuffer[y] = malloc(sizeof(CDCOLOR) * display->settings.pxWidth);
		printf("Allocated space for %d cols on row %d.\n", display->settings.pxWidth, y);
		if (display->frameBuffer[y]==NULL){
			fprintf(stderr, "Error allocating space for framebuf[%d]. Returning null.\n",y);
			for (int j = 0; j<y; j++){
				free(display->frameBuffer[j]);
			}
			free(display->frameBuffer);
			free(display);
			return NULL;
		}
	}

	init_ncurses(); // do this at the end so we dont fuck with stdout 
	display->displayWindow = createDisplayWindow(display);
	display->debugWindow = createDebugWindow(display);
	
	return display;
}

void printDisplay(CursedDisplay *display){
	if (display == NULL){
		fprintf(stderr, "Error printing display, nullptr passed.\n");
	} else {
		for (int x = 0; x<display->settings.pxWidth; x++){
			printf("- ");
		}
		printf("\n\r");
		for (int y = 0; y<display->settings.pxHeight; y++){
			for (int x =  0; x<display->settings.pxWidth; x++){
				printf("%d ",display->frameBuffer[y][x]);
			}
			printf("\n\r");
		}
	}
}

void destroyCursedDisplay(CursedDisplay *display){
	// cleanup ncurses
	end_ncurses();
	delwin(display->displayWindow);
	delwin(display->debugWindow);

	for (int y = 0; y<display->chRows; y++){
		free(display->frameBuffer[y]);
	}
	free(display->frameBuffer);
	free(display);
	display = NULL;
}

CDCOLOR getPixel(CursedDisplay *display, int py, int px){
	return display->frameBuffer[py][px];
}
void setPixel(CursedDisplay *display, int py, int px, CDCOLOR col){
	display->frameBuffer[py][px] = col;
	return; 
}

void refreshDisplay(CursedDisplay *display, float minRefreshTime){
	// ignore minrefresh time for now
	// display shite
	// 1. push pixel buffer to ncursesViewbuf
	
	// 2. update ncurses
	wrefresh(display->displayWindow);
	
	// debug shite
	// 1. push pixel buffer to ncursesViewbuf
	wrefresh(display->debugWindow);
}

void waitForInput(CursedDisplay *display){
	wgetch(display->displayWindow);
}

void writeToDebugWindow(CursedDisplay *display, int line, const char* fmt, ...){
	if (line >= DEBUG_WIN_ROWS){
		mvwprintw(display->debugWindow, 0, 0, "DEBUGLINE_ERR");
	} else {
		char fmted_msg[128];
		va_list args;
		va_start(args, fmt);
		vsprintf(fmted_msg, fmt, args);
		mvwprintw(display->debugWindow, line, 0, fmted_msg);
	}
}
