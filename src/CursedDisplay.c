#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include "cursed_display_internal.h"
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

	return display;
}

void printDisplay(CursedDisplay *display){
	if (display == NULL){
		fprintf(stderr, "Error printing display, nullptr passed.\n");
	} else {
		for (int x = 0; x<display->settings.pxWidth; x++){
			printf("- ");
		}
		printf("\n");
		for (int y = 0; y<display->settings.pxHeight; y++){
			for (int x =  0; x<display->settings.pxWidth; x++){
				printf("%d ",display->frameBuffer[y][x]);
			}
			printf("\n");
		}
	}
}

void destroyCursedDisplay(CursedDisplay *display){
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

void refreshDisplay(CursedDisplay *display, float minRefreshTime);
/* ---------------------------------------------- */
/* -------- PRIVATE FUNCTIONS -------- */
/* ---------------------------------------------- */
