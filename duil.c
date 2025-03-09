/**
 * DUCK Terminal UI implementation for unix based system
 */

#include "dui.h"

#ifdef __unix__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <ncurses.h>

static int scrollOffset = 0;
static int cursor;
static int curfetch;

static duioptions options;

/**
 * Sets the cursor position if the cursor fetch flag is activated.
 */
void cursor_update(int pos)
{
    if(!curfetch)
        return;

    cursor = MIN(pos + options.y, LINES);
    scrollOffset = MAX(0, pos + options.y - LINES);
    
    curfetch = 0;
}

void dui_init(duioptions doptions)
{
    options = doptions;
    cursor = 1 + options.y;

    initscr();

    if(has_colors()) 
    {
        use_default_colors();
        start_color();
        init_pair(1, COLOR_GREEN, -1);
    }

    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    scrollok(stdscr, TRUE);
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

    char out[310] = "";
    char sz[10];

    char path[310] = "";
    dirtree_getpath(tree, path);
    
    size((double)tree->size, sz);
    
    mvprintw(options.y, 0, "%9s %s", sz, path);

    for(int i = 0 + scrollOffset; i < tree->nfiles; i++)
    {
        dirtree *d = tree->files[i];
        
        // Size percentage for the current file
        double percent = (d->size * 1.0) / tree->size;
        
        size((double)d->size, sz);
        
        // Change console text color to highlight this file if selected
        if(i == tree->selected_file)
            attron(COLOR_PAIR(1));
        
        mvprintw(options.y + i - scrollOffset + 1, 0, "%9s %5.2lf%% [%-15.*s] %s%c", sz, percent * 100, MAX((int)round(percent * 15), 1), loadChars, d->desc.name, d->desc.type == DDIRECTORY ? '/' :  ' ');
        
        // Disable color
        attroff(COLOR_PAIR(1));
    }
}

void dui_clear(int mode)
{
    if(mode & CLEAR_ALL)
    {
        move(options.y, 0);
        clrtobot();
    }

    if(mode & CLEAR_CURSOR_OFFSET)
    {
        cursor = options.y + 1;
        scrollOffset = 0;

        // Enable the cursor fetch flag so it can be set on the next call of dui_print
        curfetch = 1;
    }
}

void dui_end()
{
    curs_set(1);
    endwin();
}

int dui_scroll_down()
{
    if(cursor < LINES - 1)
    {
        // When the cursor is on the last line scroll 
        // by adding an offset when printing
        // Also we need to clear the whole console when doing this to avoid
        // artifacts, as lines can now be of different length
        cursor++;
        return CLEAR_ATTRIBUTES;
    }
    else
    {
        scrollOffset++;
        return CLEAR_ALL;
    } 
}

int dui_scroll_up()
{
    if(cursor + scrollOffset > LINES - 1)
    {
        // When the cursor is on the last line scroll 
        // by adding an offset when printing
        // Also we need to clear the whole console when doing this to avoid
        // artifacts, as lines can now be of different length
        scrollOffset--;
        return CLEAR_ALL;
    }
    else
    {
        cursor--;
        return CLEAR_ATTRIBUTES;
    }
}

#endif 