#include <stdio.h>

#include "duck.h"

void print_tree(DirTreeNode *tree, int level)
{
    if(tree == NULL)
        return;

    int indent = level * 2;
    printf("%*s%-*s %50d\n", indent, "", 20 - indent, tree->name, tree->size);

    for(int i = 0; i < tree->files; i++)
        print_tree(tree->children[i], level + 1);
}

int comparator(const void *a, const void *b)
{
    DirTreeNode *aa = *((DirTreeNode**)a);
    DirTreeNode *bb = *((DirTreeNode**)b);

    return bb->size - aa->size;
}

void sort_tree(DirTreeNode *tree)
{
    if(tree == NULL)
        return;
    
    qsort(tree->children, tree->files, sizeof(DirTreeNode*), &comparator);

    for(int i = 0; i < tree->files; i++)
        sort_tree(tree->children[i]);
}

int main()
{
    // printf("Hello world!\n");

    DirTreeNode *root = create_dir_tree_node(".");

    int res = dir_walk(".", root);
    
    //sort_tree(root);
    print_tree(root, 0);

    printf("%d\n", res);



    return 0;
}