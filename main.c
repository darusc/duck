#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "duck.h"
#include "tui.h"

#define ARG(i, s) (strcmp(argv[i], s) == 0)

#define DUCK_DOWN 80
#define DUCK_UP 72
#define DUCK_QUIT 'q'
#define DUCK_ENTER 13
#define DUCK_BACK 8 

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

    root = dirtree_alloc(".", DDIRECTORY, NULL);
    build_dirtree(root, ".", doptions);
    
    dui_init(uioptions);

    int refresh = 0;
    while(1)
    {
        int key = getch();

        switch(key)
        {
            case DUCK_QUIT:
                dui_clear(CLEAR_ALL);
                return 0;

            case DUCK_DOWN:
                refresh = dirtree_select_next_file(root) * CLEAR_ATTRIBUTES;
                break;

            case DUCK_UP:
                refresh = dirtree_select_prev_file(root) * CLEAR_ATTRIBUTES;
                break;

            case DUCK_ENTER:
                refresh = dirtree_down(&root) * CLEAR_ALL;
                break;

            case DUCK_BACK:
                refresh = dirtree_up(&root) * CLEAR_ALL;
                break;
        }

        if(refresh)
        {
            dui_clear(refresh);
            dui_print(root);
            refresh = 0;
        }
    }

    return 0;
}