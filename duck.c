#include "duck.h"

#include <windows.h>
#include <stdio.h>

dirtree *dirtree_alloc(const char *name, enum filetype type, dirtree *parent)
{
    dirtree *node = calloc(1, sizeof(dirtree));

    strcpy(node->desc.name, name);
    node->desc.length = strlen(name);
    node->desc.type = type;
    node->selected_file = 0;

    node->nfiles = 0;
    node->size = 0;

    node->capacity = 20;
    node->files = NULL;
    node->parent = parent;
}

void dirtree_insert(dirtree *root, dirtree* node)
{
    if(root->files == NULL)
        root->files = calloc(root->capacity, sizeof(dirtree*));

    if(root->nfiles >= root->capacity)
    {
        root->capacity *= 2;
        root->files = realloc(root->files, root->capacity * sizeof(dirtree*));
    }

    root->files[root->nfiles] = node;
    root->size += node->size;

    root->nfiles++;
}

void dirtree_sort(dirtree *root, int (*comparator)(const void*, const void*))
{
    if(root == NULL)
        return;
    
    qsort(root->files, root->nfiles, sizeof(dirtree*), comparator);

    for(int i = 0; i < root->nfiles; i++)
        dirtree_sort(root->files[i], comparator);
}


int dirtree_down(dirtree **root)
{
    if((*root)->files[(*root)->selected_file]->desc.type == DDIRECTORY)
    {
        *root = (*root)->files[(*root)->selected_file];
        return 1;
    }

    return 0;
}

int dirtree_up(dirtree **root)
{
    if((*root)->parent != NULL)
    {
        *root = (*root)->parent;
        return 1;
    }

    return 0;
}


int dirtree_select_next_file(dirtree *root)
{
    if(root->selected_file < root->nfiles - 1)
    {
        root->selected_file++;
        return 1;
    }

    return 0;
}

int dirtree_select_prev_file(dirtree *root)
{
    if(root->selected_file > 0)
    {
        root->selected_file--;
        return 1;
    }

    return 0;
}


int dirtree_comp_size(const void *a, const void *b)
{
    dirtree *aa = *((dirtree**)a);
    dirtree *bb = *((dirtree**)b);

    return bb->size - aa->size;
}

int dirtree_comp_alpha(const void *a, const void *b)
{
    dirtree *aa = *((dirtree**)a);
    dirtree *bb = *((dirtree**)b);

    return strcmp(aa->desc.name, bb->desc.name);
}

int dirtree_comp_items(const void *a, const void *b)
{
    dirtree *aa = *((dirtree**)a);
    dirtree *bb = *((dirtree**)b);

    return aa->files - bb->files;
}



/**
 * Walk the given directory and builds the resulted tree.
 * Returns the size of the directory.
 */
int dir_walk(const char *dir, dirtree *tree, int all)
{
    WIN32_FIND_DATA find_data;
    HANDLE hFind = NULL;

    char path[MAX_PATH];
    sprintf(path, "%s\\*", dir);

    if((hFind = FindFirstFile(path, &find_data)) == INVALID_HANDLE_VALUE)
        return -1;

    do
    {
        // Skip if the current director is . or ..
        if(strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
            continue;

        // Skip if the file / directory is hiiden and all flag is not set
        if(!all && find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
            continue;

        int isDirectory = find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

        dirtree *n = dirtree_alloc(find_data.cFileName, isDirectory ? DDIRECTORY : DFILE, tree);
        n->size = find_data.nFileSizeLow;

        sprintf(path, "%s\\%s", dir, find_data.cFileName);
        
        // If the file is a directory recurse on it
        if(isDirectory)
            n->size = dir_walk(path, n, all);
            
        dirtree_insert(tree, n);

    } while (FindNextFile(hFind, &find_data));

    FindClose(&hFind);
    
    return tree->size;
}

void build_dirtree(dirtree *tree, const char *path, duckoptions options)
{
    dir_walk(path, tree, options.all);

    switch(options.sort)
    {
        case DSIZE:
            dirtree_sort(tree, &dirtree_comp_size);
            break;
    
        case DALPHABETIC:
            dirtree_sort(tree, &dirtree_comp_alpha);
            break;

        case DITEMS:
            dirtree_sort(tree, &dirtree_comp_items);
            break;
    }
}

int dirtree_getpath(dirtree *tree, char *path)
{
    if(tree == NULL)
        return 0;
    
    int l = dirtree_getpath(tree->parent, path);
    strcat(path, tree->desc.name);
    strcat(path, "/");

    return l + tree->desc.length + 1;
}

static const char *magnitudes = "KMGT";

void size(double size, char *out)
{
    int m = 0;
    while(size > 1024)
    {
        size /= 1024.0;
        m++;
    }

    if(m == 0)        
        sprintf(out, "%3.2lf B", size);
    else
        sprintf(out, "%3.2lf %cB", size, magnitudes[m - 1]);
}
