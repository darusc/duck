#ifndef TUI_H
#define TUI_H

#include "duck.h"

#define CLEAR_ALL           0x01
#define CLEAR_ATTRIBUTES    0x02
#define CLEAR_CURSOR_OFFSET 0x04

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