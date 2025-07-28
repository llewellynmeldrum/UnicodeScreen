#define _XOPEN_SOURCE_EXTENDED // necessary for wide chars
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <locale.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "unicode_screen_internal.h"


#define DEBUG_MAX_LC 256 
#define DEBUG_MAX_LL 256 
char debuglines[DEBUG_MAX_LC][DEBUG_MAX_LL];

int debuglc = 0;

// convinience macros
#define BEGIN_CURSES()	initscr(); 
#define END_CURSES()	endwin(); 

#define LOCALE "en_AU.UTF-8"
const wchar_t* TOP_HALF_BLOCK = L"\x2580";
const wchar_t* SPACE = L"\x0020";


/* ---------------------------------------------- */
/* ------------- PRIVATE FUNCTIONS -------------- */
/* ---------------------------------------------- */
static inline short get_pair_id(short fg, short bg){
	return (fg+1) * (N_CDCOLORS+1) + (bg+1);
}
int getDisplayHeight(UnicodeScreen *d){
	return d->settings.pxHeight;
}

int getDisplayWidth(UnicodeScreen *d){
	return d->settings.pxWidth;
}
void initColorPairs(short ncolors){
	for (short fg = -1; fg < ncolors; ++fg)
	for (short bg = -1; bg < ncolors; ++bg) {
		short id = get_pair_id(fg, bg);
		init_pair(id, fg, bg);
	    }

}

void init_ncurses(UnicodeScreen *display){
	setlocale(LC_ALL, "");
	BEGIN_CURSES(); // begin curses mode
	noecho(); // dont care about input for now 
	if (!has_colors()){
		destroyCursedDisplay(display);
		fprintf(stderr, "\n\rERROR: Your terminal does not suppport colors. Terminating.\n\r");
		exit(EXIT_FAILURE);
	} else {
		start_color();
		use_default_colors();
	}
	initColorPairs(N_CDCOLORS);
}

WINDOW *createDisplayWindow(UnicodeScreen *display){
	const int startX = 0;
	const int startY = 0;
	WINDOW * displayWindow = newwin(display->chRows, display->chCols, startY, startX);
	printf("\r\n NCURSES WIN: %d ROWS, %d COLS\n\r", display->chCols, display->chRows);
	return displayWindow;
}

WINDOW *createDebugWindow(UnicodeScreen *display){
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

UnicodeScreen *createCursedDisplay(UnicodeScreenSettings settings){
	if (settings.pxHeight<=0 || settings.pxHeight%2!=0 || settings.pxWidth<=0){
		fprintf(stderr, "Error creating display, bad input for display settings. Returning null.\n");
		return NULL;
	}

	UnicodeScreen *display = malloc(sizeof(UnicodeScreen));
	if (display==NULL){
		fprintf(stderr, "Error allocating space for display. Returning null.\n");
		return NULL;
	}
	display->settings = settings;
	display->chRows = settings.pxHeight / 2;
	display->chCols = settings.pxWidth;


	display->frameBuffer = malloc(sizeof(CDCOLOR *) * display->settings.pxHeight);
	if (display->frameBuffer==NULL){
		fprintf(stderr, "Error allocating space for display framebuf. Returning null.\n");
		free(display);
		return NULL; 
	}

	for (int y = 0; y<display->settings.pxHeight; y++){
		display->frameBuffer[y] = malloc(sizeof(CDCOLOR) * display->settings.pxWidth);
	//	printf("Allocated space for %d cols on row %d.\n", display->settings.pxWidth, y);
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

	init_ncurses(display); // do this at the end so we dont ruin stdout 
	display->displayWindow = createDisplayWindow(display);
	display->debugWindow = createDebugWindow(display);
	return display;
}

void printDisplay(UnicodeScreen *display){
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

void destroyCursedDisplay(UnicodeScreen *display){
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

CDCOLOR getPixel(UnicodeScreen *display, int py, int px){
	return display->frameBuffer[py][px];
}
void setPixel(UnicodeScreen *display, int py, int px, CDCOLOR col){
	display->frameBuffer[py][px] = col;
	return; 
}

void refreshDisplay(UnicodeScreen *display, float minRefreshTime){
	for (int py = 0; py<display->settings.pxHeight-1; py+=2){
		for (int px = 0; px<display->settings.pxWidth; px++){
			CDCOLOR top_pixel_color = getPixel(display, py, px);
			CDCOLOR bot_pixel_color = getPixel(display, py+1, px);
			short pair_id = get_pair_id(top_pixel_color, bot_pixel_color);
			int row = py/2;
			int col = px;
			wattron(display->displayWindow,COLOR_PAIR(pair_id));
			mvwaddwstr(display->displayWindow, row, col, TOP_HALF_BLOCK);
			wattroff(display->displayWindow, COLOR_PAIR(pair_id));
		}
	}
	wrefresh(display->debugWindow);
}

void waitForInput(UnicodeScreen *display){
	wgetch(display->displayWindow);
}


static void dump_wstr(const wchar_t *ws){
    for (size_t i = 0; ws[i] != L'\0'; ++i) {
        printf("U+%04X\'%lc\'\r", (unsigned int)ws[i],  ws[i]);
    }
}
void dump_display_window(UnicodeScreen *display){
	printf("\n\r----- DISPLAY DUMP -----\n\r");
	wchar_t thb[2];
	wchar_t sp[2];
	printf("\rFOR REFERENCE:\r");
	printf("TOP_HALF_BLOCK = "); dump_wstr(TOP_HALF_BLOCK);
	printf("SPACE (SP) = "); dump_wstr(SPACE);
	for(int row = 0; row<display->chRows; row++){
		for (int col = 0; col<display->chCols; col++){
			cchar_t cch; 
			mvwin_wch(display->displayWindow, row, col, &cch);
			wchar_t wc[2];
			attr_t attr; short col;
			getcchar(&cch, wc, &attr, &col, NULL);
			printf("U+%04X ", wc[0]);
		}
		printf("\r\n");
	}
	for(int row = 0; row<display->chRows; row++){
		for (int col = 0; col<display->chCols; col++){
			cchar_t cch; 
			mvwin_wch(display->displayWindow, row, col, &cch);
			wchar_t wc[2];
			attr_t attr; short col;
			getcchar(&cch, wc, &attr, &col, NULL);
			printf("%lc", wc[0]);
		}
		printf("\r\n");
	}
}
void writeToDebugWindow(UnicodeScreen *display, int line, const char* fmt, ...){
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
void debug_log_fline(char* fmt, ...){
	va_list args; 
	va_start(args, fmt);
	if (debuglc==DEBUG_MAX_LC){
		vsprintf(debuglines[debuglc], "END OF DEBUG LINES REACHED", args);
		va_end(args);
		return;
	}
	vsprintf(debuglines[debuglc], fmt, args);
	va_end(args);
	debuglc++;
}

void debug_log_all(){
	debug_log_fline("COLOR_PAIRS = %d", COLOR_PAIRS);
	debug_log_fline("id\tfg\tbg");

	short ncolors = 8; 
	for (short fg = -1; fg < ncolors; ++fg)
	for (short bg = -1; bg < ncolors; ++bg){
	short id = get_pair_id(fg, bg);
	//	short id = -1;
		debug_log_fline("%03hd\t%03hd\t%03hd", id, fg, bg);
	}
}

void debug_print_log(){
	for (int line = 0; line<debuglc; line++){
		printf("%s\n\r", debuglines[line]);
	}
}
