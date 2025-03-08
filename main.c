#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __unix__
    #include <ncurses.h>
#endif

#include "duck.h"
#include "dui.h"

#define ARG(i, s) (strcmp(argv[i], s) == 0)

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

dirtree* root = NULL;

int main(int argc, char **argv)
{
    if(argc < 2 || ARG(1, "-h") || ARG(1, "--help") || argv[1][0] == '-')
    {
        printf("duck v1.0 - Disk usage analysis tool\nhttps://github.com/darusc/duck\n\n");
        printf("Usage:\n duck <directory> [options]\n\n");
        printf("Options:\n");
        printf(" -h,  --help                 Print this info\n");
        printf(" -ni, --nointeractive        Disables interactive terminal output. Shows all files directly.\n");
        printf(" -nf, --nofullscreen         Disables fullscreen terminal output.\n");
        printf(" -a,  --all                  Shows all files (including hidden files)\n");
        printf(" -s,  --sort <method>        Sort by (size is default)\n");
        printf("      methods: <size|alphabetic|items|>\n");         
        printf("\n\n");
    
        return 0;
    }

    duioptions uioptions = {1, 1};
    duckoptions doptions = {DSIZE, 0};

    for(int i = 2; i < argc; i++)
    {
        if(ARG(i, "-ni") || ARG(i, "--nointeractive"))
            uioptions.interactive = 0;

        if(ARG(i, "-nf") || ARG(i, "--nofullscreen"))
            uioptions.fullscreen = 0;

        if(ARG(i, "-a") || ARG(i, "--all"))
            doptions.all = 1;

        if(ARG(i, "-s") || ARG(i, "--sort"))
        {
            if(ARG(i + 1, "size"))
                doptions.sort = DSIZE;
            else if(ARG(i + 1, "alphabetic"))
                doptions.sort = DALPHABETIC;
            else if(ARG(i + 1, "items"))
                doptions.sort = DITEMS;
        }
    }

    root = dirtree_alloc(argv[1], DDIRECTORY, NULL);
    build_dirtree(root, argv[1], doptions);
    
    dui_init(uioptions);

    int clrmode = 0;
    dui_print(root);
    while(1)
    {
        int key = getch();
            
        switch(key)
        {
            case DUCK_QUIT:
                dui_end();
                return 0;

            case DUCK_DOWN:
                if(dirtree_select_next_file(root))
                {
                    clrmode = dui_scroll_down();
                }
                break;

            case DUCK_UP:
                if(dirtree_select_prev_file(root))
                {
                    clrmode = dui_scroll_up();
                }
                break;

            case DUCK_ENTER:
                clrmode = dirtree_down(&root) * (CLEAR_ALL | CLEAR_CURSOR_OFFSET);
                break;

            case DUCK_BACK:
                clrmode = dirtree_up(&root) * (CLEAR_ALL | CLEAR_CURSOR_OFFSET);
                break;
        }

        if(clrmode)
        {
            #ifdef __unix__
                dui_clear(clrmode);
                dui_print(root);
                refresh();
            #else
                dui_clear(clrmode);
                dui_print(root);
            #endif

            clrmode = 0;
        }
    }

    return 0;
}