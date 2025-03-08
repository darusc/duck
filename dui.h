#ifndef DUI_H
#define DUI_H

#include "duck.h"

#ifdef __unix__
    #define DUCK_DOWN KEY_DOWN
    #define DUCK_UP KEY_UP
    #define DUCK_QUIT 'q'
    #define DUCK_ENTER 10
    #define DUCK_BACK KEY_BACKSPACE
#else
    #define DUCK_DOWN 80
    #define DUCK_UP 72
    #define DUCK_QUIT 'q'
    #define DUCK_ENTER 13
    #define DUCK_BACK 8 
#endif

#define CLEAR_ALL           0x01
#define CLEAR_ATTRIBUTES    0x02
#define CLEAR_CURSOR_OFFSET 0x04

static const char loadChars[15] = {219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219};

typedef struct duioptions 
{
    /**
     * Intercative mode.
     * (enabled) Displays only the direct children of a directory and allows navigation
     * (disabled) Displays all files in a tree like structure
     */
    int interactive;

    /**
     * Fullscreen mode
     * (enabled) Clears the terminal before printing
     * (disabled) Doesn't clear the terminal, the output is printed normally
     */
    int fullscreen;
} duioptions;

void dui_init(duioptions options);
void dui_print(dirtree *tree);

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