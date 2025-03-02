#include <stdio.h>
#include <conio.h>

#include "duck.h"
#include "tui.h"

int main()
{
    dirtree *root = dirtree_create(".", DDIRECTORY, NULL);

    int res = dir_walk(".", root);
    
    dirtree_sort(root, &comparator_size);

    init();
    dirtree_print(root);

    int redraw = 0;
    while(1)
    {
        int key = getch();

        if(key == 80)
        {
            if(root->selected_file < root->nfiles - 1)
            {
                root->selected_file++;
                redraw = CLEAR_NO_CLS;
            }
        }

        if(key == 72)
        {
            if(root->selected_file > 0)
            {
                root->selected_file--;
                redraw = CLEAR_NO_CLS;
            }
        }

        if(key == 13)
        {
            if(root->files[root->selected_file]->desc.type == DDIRECTORY)
            {
                root = root->files[root->selected_file];
                redraw = CLEAR_CLS;
            }
        }

        if(key == 8)
        {
            if(root->parent != NULL)
            {
                root = root->parent;
                redraw = CLEAR_CLS;
            }
        }

        if(redraw)
        {
            clear(redraw);
            dirtree_print(root);
            redraw = 0;
        }
    }


    return 0;
}