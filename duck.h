#ifndef DUCK_H
#define DUCK_H

#include <stdint.h>
#include <windows.h>

enum Filetype
{
    File,
    Directory
};

typedef struct DirTreeNode
{
    char name[MAX_PATH];
    enum Filetype type;

    uint32_t size;
    uint32_t files;

    uint32_t capacity;
    struct DirTreeNode **children;
} DirTreeNode;

DirTreeNode* create_dir_tree_node(const char* name);
void dir_tree_insert(DirTreeNode *root, DirTreeNode* node);

int dir_walk(const char *dir, DirTreeNode *tree);

#endif