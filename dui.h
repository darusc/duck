#ifndef DUI_H
#define DUI_H

#include "duck.h"

#include <stdio.h>
#include <math.h>

#ifdef __unix__
    #include <ncurses.h>

    #define DUCK_DOWN KEY_DOWN
    #define DUCK_UP KEY_UP
    #define DUCK_QUIT 'q'
    #define DUCK_ENTER 10
    #define DUCK_BACK KEY_BACKSPACE
#else
    #include <windows.h>

    #define DUCK_DOWN 80
    #define DUCK_UP 72
    #define DUCK_QUIT 'q'
    #define DUCK_ENTER 13
    #define DUCK_BACK 8 
#endif

#define CLEAR_ALL           0x01
#define CLEAR_ATTRIBUTES    0x02
#define CLEAR_CURSOR_OFFSET 0x04
#define CLEAR_END     0x08

static const char headerMsg[] = " duck ~ Use arrow keys to navigate, backspace - go to parent directory, enter - go to child directory";

static const char loadChars[16] = {219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 0};

typedef struct duioptions 
{
    /**
     * Fullscreen mode
     * (enabled) Clears the terminal before printing
     * (disabled) Doesn't clear the terminal, the output is printed normally
     */
    int fullscreen;

    /**
     * Y coord to start drawing UI from
     */
    int y;

    /**
     * Show item count
     */
    int count;
} duioptions;

void dui_init(duioptions options);
void dui_print(dirtree *tree);

void dui_header();

static inline void dui_benchmark_print(benchmark bench)
{
    #ifdef __unix__
        printw("Files: %d  Directories: %d\n", bench.files, bench.directories);
        printw("Discovery time: %.3lf sec\n", bench.dtime);
        printw("Total time: %.3lf sec\n", bench.dtime);
    #else
        COORD coord = {0, 1};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        printf("Files: %d  Directories: %d\n", bench.files, bench.directories);
        printf("Discovery time: %.3lf sec\n", bench.dtime);
        printf("Total time: %.3lf sec\n", bench.dtime);
    #endif
}

static inline void dui_tree_root_to_string(dirtree *root, char *str, duioptions options)
{
    static char sz[10];
    char path[300] = "";
    dirtree_getpath(root, path);
    
    size((double)root->size, sz);

    if(options.count)
        sprintf(str, "%9s        items: %6d %s", sz, root->nfiles, path);
    else
        sprintf(str, "%9s %s", sz, path);
}

static inline void dui_tree_child_to_string(dirtree *node, dirtree *parent, char *str, duioptions options)
{
    static char sz[10];

    // Space percentage for the current file
    double percent = (node->size * 1.0) / parent->size;
        
    size((double)node->size, sz);

    if(options.count)
        sprintf(str, "%9s %5.2lf%% items: %6d [%-15.*s] %s%c", sz, percent * 100, node->nfiles, (int)round(percent * 15), loadChars, node->desc.name, node->desc.type == DDIRECTORY ? '/' :  ' ');
    else
        sprintf(str, "%9s %5.2lf%% [%-15.*s] %s%c", sz, percent * 100, (int)round(percent * 15), loadChars, node->desc.name, node->desc.type == DDIRECTORY ? '/' :  ' ');
}

/**
 * Clears the part of the console where output was printed. (The whole console for
 * fullscreen mode and only the used part when in non fullscreen mode)
 * 
 * CLEAR_ALL - clears the whole screen by flooding with spaces ' ' and removing color attributes
 * 
 * CLEAR_ATTRIBUTES - clears only the color attributes
 * 
 * @param mode CLEAR_ALL or CLEAR_ATTRIBUTES
 */
void dui_clear(int mode);

void dui_end();

/**
 * Moves the cursor down. Returns the clear method that needs to be used
 */
int dui_scroll_down();

/**
 * Moves the cursor up. Returns the clear method that needs to be used
 */
int dui_scroll_up();

#endif