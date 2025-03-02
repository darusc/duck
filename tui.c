#include "tui.h"

#include <stdio.h>
#include <windows.h>

#ifdef WIN32

/**
 * Attributes used to set console text color.
 * Size set to MAX_PATH (260) - the maximum size of a file/directory name is 260 characters on WIN32 systems
 */ 
static WORD colorAttributes[MAX_PATH];

static HANDLE hConsole;
static CONSOLE_SCREEN_BUFFER_INFO csbi;

static duioptions options;

void dui_init(duioptions doptions)
{
    // Initialize the color attributes
    for(int i = 0; i < MAX_PATH; i++)
        colorAttributes[i] = FOREGROUND_GREEN;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    options = doptions;

    if(options.fullscreen)
        system("cls");
}

void dui_print(dirtree *tree)
{
    if(tree == NULL)
        return;

    DWORD bytes;
    
    for(int i = 0; i < tree->nfiles; i++)
    {
        dirtree *d = tree->files[i];

        COORD coord = {0, (short)i};
        if(!options.fullscreen)
            coord.Y += csbi.dwCursorPosition.Y;
        
        if(i == tree->selected_file)
        {
            WriteConsoleOutputAttribute(hConsole, colorAttributes, d->desc.length, coord, &bytes);
        }
    
        WriteConsoleOutputCharacterA(hConsole, d->desc.name, d->desc.length, coord, &bytes);
    }
}

void dui_clear(int mode)
{
    COORD coord = {0, 0};
    DWORD bytes;

    if(!options.fullscreen)
        coord.Y = csbi.dwCursorPosition.Y;

    if(mode == CLEAR_ALL)
    {
        FillConsoleOutputCharacter(hConsole, TEXT(' '), csbi.dwSize.X * csbi.dwSize.Y, coord, &bytes);
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &bytes);
    }
    else if(mode == CLEAR_ATTRIBUTES)
    {
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &bytes);
    }
}

#endif