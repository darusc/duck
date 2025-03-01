#include "duck.h"

#include <windows.h>
#include <stdio.h>

DirTreeNode *create_dir_tree_node(const char *name)
{
    DirTreeNode *node = calloc(1, sizeof(DirTreeNode));
    strcpy(node->name, name);
    node->files = 0;
    node->size = 0;
    node->capacity = 20;
    node->children = NULL;
}

void dir_tree_insert(DirTreeNode *root, DirTreeNode* node)
{
    if(root->children == NULL)
        root->children = calloc(root->capacity, sizeof(DirTreeNode*));

    if(root->files >= root->capacity)
    {
        root->capacity *= 2;
        root->children = realloc(root->children, root->capacity * sizeof(DirTreeNode*));
    }

    root->children[root->files] = node;
    root->size += node->size;

    root->files++;
}

int dir_walk(const char *dir, DirTreeNode *tree)
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

        DirTreeNode *n = create_dir_tree_node(find_data.cFileName);
        n->size = find_data.nFileSizeLow;

    
        sprintf(path, "%s\\%s", dir, find_data.cFileName);
        
        if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            n->size = dir_walk(path, n);

        dir_tree_insert(tree, n);

    } while (FindNextFile(hFind, &find_data));

    FindClose(&hFind);
    
    return tree->size;
}
