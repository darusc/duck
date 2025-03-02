#ifndef DUCK_H
#define DUCK_H

#include <stdint.h>
#include <windows.h>

enum filetype
{
    File,
    Directory
};

typedef struct dirtree_desc
{
    enum filetype type;
    char name[MAX_PATH];
    uint8_t length;
} dirtree_desc;

typedef struct dirtree
{
    struct dirtree_desc desc;
    uint32_t selected_child;

    uint32_t size;
    uint32_t nfiles;

    uint32_t capacity;
    struct dirtree **files;
} dirtree;


dirtree* dirtree_create(const char* name);

void dirtree_insert(dirtree *root, dirtree* node);
void dirtree_sort(dirtree *root, int (*comparator)(const void*, const void*));

/**
 * Walk the given directory and builds the resulted tree.
 * Returns the size of the directory.
 */
int dir_walk(const char *dir, dirtree *tree);

int comparator_size(const void *a, const void *b);
// int comparator_alpha(const void *a, const void *b);

#endif