#ifndef TUI_H
#define TUI_H

#include "duck.h"

#define CLEAR_CLS 1
#define CLEAR_NO_CLS 2

void init();

void dirtree_print(dirtree *tree);
void clear();

#endif