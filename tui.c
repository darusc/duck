#include "tui.h"

#include <stdio.h>
#include <windows.h>
#include <math.h>

#ifdef WIN32

/**
 * Attributes used to set console text color.
 * Size set to MAX_PATH (260) - the maximum size of a file/directory name is 260 characters on WIN32 systems
 */ 
static WORD colorAttributes[MAX_PATH];

static HANDLE hConsole;
static CONSOLE_SCREEN_BUFFER_INFO csbi;

static int scrollOffset = 0;
static int cursor;

static duioptions options;

static char loadChars[10];

void dui_init(duioptions doptions)
{
    options = doptions;        

    if(options.fullscreen)
        system("cls");    

    // Initialize the color attributes
    for(int i = 0; i < MAX_PATH; i++)
        colorAttributes[i] = FOREGROUND_GREEN;

    for(int i = 0; i < 10; i++)
        loadChars[i] = 219;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    cursor = csbi.dwCursorPosition.Y;

    // Disable the cursor
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(hConsole, &cci);

    cci.bVisible = 0;
    SetConsoleCursorInfo(hConsole, &cci);
}

void dui_print(dirtree *tree)
{
    if(tree == NULL)
        return;

    DWORD bytes;

    COORD c = {0, 0};

    for(int i = 0 + scrollOffset; i < tree->nfiles; i++)
    {
        dirtree *d = tree->files[i];

        COORD coord = {0, (short)i - scrollOffset};
        if(!options.fullscreen)
            coord.Y += csbi.dwCursorPosition.Y;
        
        char out[300];
        double percent = (d->size * 1.0) / tree->size;
        sprintf(out, "%10d %5.2lf%% [%-15.*s] %s", d->size, percent * 100, max((int)round(percent * 15), 1), loadChars, d->desc.name);
        int len = strlen(out);

        if(i == tree->selected_file)
        {
            WriteConsoleOutputAttribute(hConsole, colorAttributes, len, coord, &bytes);
        }
    
        WriteConsoleOutputCharacterA(hConsole, out, len, coord, &bytes);
    }
}

void dui_clear(int mode)
{
    COORD coord = {0, 0};
    DWORD bytes;

    if(!options.fullscreen)
        coord.Y = csbi.dwCursorPosition.Y;

    if(mode & CLEAR_ALL)
    {
        FillConsoleOutputCharacter(hConsole, TEXT(' '), csbi.dwSize.X * csbi.dwSize.Y, coord, &bytes);
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &bytes);
    }
    
    if (mode & CLEAR_CURSOR_OFFSET)
    {
        cursor = csbi.dwCursorPosition.Y;
        scrollOffset = 0;
    }

    if(mode & CLEAR_ATTRIBUTES)
    {
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &bytes);
    }        
}

void dui_end()
{
    dui_clear(CLEAR_ALL);
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(hConsole, &cci);

    cci.bVisible = 1;
    SetConsoleCursorInfo(hConsole, &cci);   
}

int dui_scroll_down()
{
    if(cursor < csbi.dwMaximumWindowSize.Y - 1)
    {
        cursor++;
        return CLEAR_ATTRIBUTES;
    }
    else
    {
        // When the cursor is on the last line scroll 
        // by adding an offset when printing
        // Also we need to clear the whole console when doing this to avoid
        // artifacts, as lines can now be of different length
        scrollOffset++;
        return CLEAR_ALL;
    }
}

int dui_scroll_up()
{
    if(cursor + scrollOffset > csbi.dwMaximumWindowSize.Y - 1)
    {
        // When the cursor is on the last line scroll 
        // by adding an offset when printing
        // Also we need to clear the whole console when doing this to avoid
        // artifacts, as lines can now be of different length
        scrollOffset--;
        return CLEAR_ALL;
    }
    else if(cursor > 0)
    {
        cursor--;
        return CLEAR_ATTRIBUTES;
    }
}

#endif