/**
 * DUCK Terminal UI implementation for win32 system
 */

#include "dui.h"

#ifndef __unix__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <windows.h>

static int scrollOffset = 0;
static int cursor;
static int curfetch;

static duioptions options;

/**
 * Attributes used to set console text color.
 * Size set to MAX_PATH (260) - the maximum size of a file/directory name is 260 characters on WIN32 systems
 */ 
static WORD colorAttributes[MAX_PATH];

static HANDLE hConsole;
static CONSOLE_SCREEN_BUFFER_INFO csbi;

/**
 * Sets the cursor position if the cursor fetch flag is activated.
 */
void cursor_update(int pos)
{
    if(!curfetch)
        return;

    cursor = min(pos + options.y, csbi.dwMaximumWindowSize.Y);
    scrollOffset = max(0, pos + options.y - csbi.dwMaximumWindowSize.Y);
    
    curfetch = 0;
}

void dui_init(duioptions doptions)
{
    options = doptions;        

    system("cls");    

    // Initialize the color attributes
    for(int i = 0; i < MAX_PATH; i++)
        colorAttributes[i] = BACKGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    cursor = csbi.dwCursorPosition.Y + options.y + 1;

    // Disable the cursor
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(hConsole, &cci);

    cci.bVisible = 0;
    SetConsoleCursorInfo(hConsole, &cci);
}

void dui_header()
{
    COORD coord = {0, 0};
    DWORD bytes;

    WORD *background = (WORD*)malloc(csbi.dwMaximumWindowSize.X * sizeof(WORD));
    for(int i = 0; i < csbi.dwMaximumWindowSize.X; i++)
        background[i] = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
    
    WriteConsoleOutputAttribute(hConsole, background, csbi.dwMaximumWindowSize.X, coord, &bytes);

    WriteConsoleOutputCharacter(hConsole, headerMsg, strlen(headerMsg), coord, &bytes);
}

void dui_print(dirtree *tree)
{
    if(tree == NULL)
        return;

    /**
     * Sets the cursor position to be on the same line with
     * the highlighted file when navigating up or down the directory tree
     */
    cursor_update(tree->selected_file + 1);

    DWORD bytes;
    COORD coord = {0, options.y};

    char out[300] = "";
    char sz[10];
    
    // Print the size and current director path on the first line
    char path[300] = "";
    dirtree_getpath(tree, path);
    
    size((double)tree->size, sz);
    sprintf(out, "%9s %s", sz, path);

    int len = strlen(out);
    WriteConsoleOutputCharacterA(hConsole, out, len, coord, &bytes);

    for(int i = 0 + scrollOffset; i < tree->nfiles; i++)
    {
        dirtree *d = tree->files[i];

        coord.Y = options.y + (short)i - scrollOffset + 1;
        
        // Space percentage for the current file
        double percent = (d->size * 1.0) / tree->size;
        
        size((double)d->size, sz);
        sprintf(out, "%9s %5.2lf%% [%-15.*s] %s%c", sz, percent * 100, (int)round(percent * 15), loadChars, d->desc.name, d->desc.type == DDIRECTORY ? '/' :  ' ');
        
        len = strlen(out);

        // Change console text color to highlight this file if selected
        if(i == tree->selected_file)
            WriteConsoleOutputAttribute(hConsole, colorAttributes, len, coord, &bytes);
    
        WriteConsoleOutputCharacterA(hConsole, out, len, coord, &bytes);
    }
}

void dui_clear(int mode)
{
    COORD coord = {0, options.y};
    DWORD bytes;

    if(mode & CLEAR_ALL)
    {
        FillConsoleOutputCharacter(hConsole, TEXT(' '), csbi.dwSize.X * csbi.dwSize.Y, coord, &bytes);
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &bytes);
    }

    if(mode & CLEAR_END)
    {
        coord.Y = 0;
        FillConsoleOutputCharacter(hConsole, TEXT(' '), csbi.dwSize.X * csbi.dwSize.Y, coord, &bytes);
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &bytes);
    }
    
    if (mode & CLEAR_CURSOR_OFFSET)
    {
        cursor = csbi.dwCursorPosition.Y + options.y + 1;
        scrollOffset = 0;

        // Enable the cursor fetch flag so it can be set on the next call of dui_print
        curfetch = 1;
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

    COORD coord = {0, 0};
    SetConsoleCursorPosition(hConsole, coord);
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