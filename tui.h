#ifndef TUI_H
#define TUI_H

#include "duck.h"

#define CLEAR_ALL 1
#define CLEAR_ATTRIBUTES 2

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
void dui_clear(int mode);

#endif