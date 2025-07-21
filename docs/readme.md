# curseD - an n**curse**s **D**isplay

## public functions
### CursedDisplay createDisplay(CD_Settings publicSettings);
To create a display, the user simply specifies the dimensions of the screen (in pixels).
- for a display {.pxHeight = x, .pxWidth = y}, the ncurses terminal window will be x columns by (y / 2) rows. 

This function creates the display and passes the user an opaque pointer,
### set and getPixel 
Self explanatory.

### void refreshDisplay(CursedDisplay cdptr, float s_minRefreshTime)
Refreshes the display, if s_minRefreshTime seconds have passed.
If the 2nd argument is passed as zero, the refresh rate is as fast as possible. 

## tools

### Makefile
idk its a pretty standard makefile - credit to chat gippity for the formatting 

### bear
Tool to auto generate `compile_commands.json` for LSP to understand the include paths and whatnot. Otherwise it freaks out with this sort of directory. Theres probably a nvim plugin for this but im lazy.
