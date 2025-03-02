#include <stdio.h>
#include <conio.h>

#include "duck.h"
#include "tui.h"

int main()
{
    dirtree *root = dirtree_create(".");

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
            if(root->selected_child < root->nfiles - 1)
            {
                root->selected_child++;
                redraw = 1;
            }
        }

        if(key == 72)
        {
            if(root->selected_child > 0)
            {
                root->selected_child--;
                redraw = 1;
            }
        }

        if(redraw)
        {
            clear();
            dirtree_print(root);
            redraw = 0;
        }
    }


    return 0;
}