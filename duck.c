#include "duck.h"

#include <windows.h>
#include <stdio.h>

dirtree *dirtree_create(const char *name)
{
    dirtree *node = calloc(1, sizeof(dirtree));

    strcpy(node->desc.name, name);
    node->desc.length = strlen(name);
    node->selected_child = 0;

    node->nfiles = 0;
    node->size = 0;

    node->capacity = 20;
    node->files = NULL;
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

int dir_walk(const char *dir, dirtree *tree)
{

    WIN32_FIND_DATA find_data;
    HANDLE hFind = NULL;

    char path[MAX_PATH];
    sprintf(path, "%s\\*", dir);

    if((hFind = FindFirstFile(path, &find_data)) == INVALID_HANDLE_VALUE)
        return -1;

    do
    {
        if(strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
            continue;

        dirtree *n = dirtree_create(find_data.cFileName);
        n->size = find_data.nFileSizeLow;

        sprintf(path, "%s\\%s", dir, find_data.cFileName);
        
        if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            n->size = dir_walk(path, n);

        dirtree_insert(tree, n);

    } while (FindNextFile(hFind, &find_data));

    FindClose(&hFind);
    
    return tree->size;
}

int comparator_size(const void *a, const void *b)
{
    dirtree *aa = *((dirtree**)a);
    dirtree *bb = *((dirtree**)b);

    return bb->size - aa->size;
}
