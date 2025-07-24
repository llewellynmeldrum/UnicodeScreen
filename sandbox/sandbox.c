#define _XOPEN_SOURCE_EXTENDED // necessary for wide chars
#include <ncurses.h>
#include <locale.h>
#include <wchar.h>
const wchar_t UHB = L'\x2580';
const wchar_t LHB = L'\x2584';
int main(){
	setlocale(LC_ALL, "");
	initscr();
	noecho();


	for (int i = 0; i<=9; i++){
		printw("%d",i);
	}

	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	int framecount = 0;
	for (int y= 1; y<=10; y++){
		for (int x = 0; x<=10; x++){
			mvaddwstr(y,x,L" "); 
			mvaddwstr(y,x,&LHB);
		}
	}
	framecount++;
	refresh();


	getch();
	endwin();
}
