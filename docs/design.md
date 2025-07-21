# design
This project intends to create an opaque interface for creating dynamically sized ncurses array, which can be accessed through the following public interfaces:
```c
    CursedDisplay* init_cursed_display(CD_Config, );
    setPixel(CD)
```

