## Ive run into a problem.
The problem has to do with wide characters behaving weirdly when many are drawn at once.

In my earlier testing, I believe that refreshing the window inbetween waddwstr calls seemed to fix the issue.

To better understand the issue, im going to try and figure out how to directly read from the ncurses buffer/screen if possible.


So after investigating the issue, I have found that with a height of 10, the program breaks if I try to write more than 5 columns at once. 
Going to test refreshing after writing 5 columns


Ok so that worked. Now I wonder if its just the raw amount of chars.

If this is the case, the limit is somewhere around 50 wchars. Lets test this with a terminal thats 25 wide


### After testing, it seems that drawing >5 wchar symbols horizontally without a refresh breaks ncurses.


ok so im officially lost. i think ive broken the colors somehow, or they were alwasy broken. Next steps are to test with regular chars, get that working and then keep working on the wchars problem.
