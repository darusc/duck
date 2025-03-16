#include "duck.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __unix__
    #ifndef _DEFAULT_SOURCE
        #define _DEFAULT_SOURCE
    #endif
    
    #include <sys/stat.h>
    
    #include <stddef.h>
    #include <dirent.h>
#endif

benchmark bench = {0, 0, 0, 0};

dirtree *dirtree_alloc(const char *name, enum filetype type, dirtree *parent)
{
    dirtree *node = calloc(1, sizeof(dirtree));

    strcpy(node->desc.name, name);
    node->desc.length = strlen(name);
    node->desc.type = type;
    node->desc.hidden = 0;
    node->selected_file = 0;

    node->nfiles = 0;
    node->size = 0;

    node->capacity = 20;
    node->files = NULL;
    node->parent = parent;

    return node;
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

    // Return like this instead of bb->size - aa->size
    // because the comparator must return an int
    // but dirtree.size is size_t aka unsigned long long
    if(bb->size > aa->size)
    return 1;
    else if(bb->size < aa->size)
        return -1;
    else
        return 0;
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

    return bb->nfiles - aa->nfiles;
}

void size(double size, char *out)
{
    int m = 0;
    while(size > 1000)
    {
        size /= 1024.0;
        m++;
    }

    if(m == 0)        
        sprintf(out, "%3.2lf B", size);
    else
        sprintf(out, "%3.2lf %cB", size, magnitudes[m - 1]);
}

int dirtree_getpath(dirtree *tree, char *path)
{
    if(tree == NULL)
        return 0;
    
    int l = dirtree_getpath(tree->parent, path);
    
    if(tree->parent == NULL)
        strcpy(path, tree->desc.name);
    else
        strcat(path, tree->desc.name);
    strcat(path, "/");

    return l + tree->desc.length + 1;
}

/**
 * Applies filter options on the given file.
 * @return 0 if the file should be skipped.
 */
int filter(dirtree *node, duckoptions options)
{
    if(options.hide && node->desc.hidden)
        return 0;

    if(options.nexts)
    {
        if(options.include && node->desc.type == DDIRECTORY)
            return node->nfiles > 0;

        if(node->desc.type == DDIRECTORY && node->nfiles == 0)
            return 0;

        int ext_in_list = 0;
        const char *ext = strrchr(node->desc.name, '.');
        
        if(ext != NULL)
        {
            for(int i = 0; i < options.nexts; i++)
            {
                if(strcmp(ext + 1, options.extenstions[i]) == 0)
                {
                    ext_in_list = 1;
                    break;
                }
            }
        }

        if(options.include && !ext_in_list)
            return 0;

        if(options.exclude && ext_in_list)
            return 0;
    }

    return 1;
}

#ifdef __unix__
    /**
    * Walk the given directory and builds the resulted tree.
    * Returns the size of the directory.
    * UNIX IMPLEMENTATION
    */
    size_t dir_walk(const char *dir, dirtree *tree, duckoptions options)
    {
        char path[MAX_PATH];

        // Return if the current path is invalid
        DIR *d = opendir(dir);
        if(d == NULL)
            return 0;
        
        struct dirent *dent;
        struct stat fstat;

        while((dent = readdir(d)) != NULL)
        {
            // Skip if the current director is . or ..
            if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
                continue;

            int isDirectory = dent->d_type == 4; //DT_DIR

            sprintf(path, "%s/%s", dir, dent->d_name);

            stat(path, &fstat);

            dirtree *n = dirtree_alloc(dent->d_name, isDirectory ? DDIRECTORY : DFILE, tree);
            n->size = fstat.st_size;

            if(isDirectory)
                n->size = dir_walk(path, n, options);

            if(!filter(n, options))
                continue;

            dirtree_insert(tree, n);

            BENCHMARK_INC_FILES(bench, isDirectory);
        }

        closedir(d);

        return tree->size;
    }
#else
    /**
    * Walk the given directory and builds the resulted tree.
    * Returns the size of the directory.
    * WIN32 IMPLEMENTATION
    */
    size_t dir_walk(const char *dir, dirtree *tree, duckoptions options)
    {
        WIN32_FIND_DATA find_data;
        HANDLE hFind = NULL;

        char path[MAX_PATH];
        sprintf(path, "%s\\*", dir);

        if((hFind = FindFirstFile(path, &find_data)) == INVALID_HANDLE_VALUE)
            return 0;

        do
        {
            // Skip if the current director is . or ..
            if(strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
                continue;

            int isDirectory = find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

            dirtree *n = dirtree_alloc(find_data.cFileName, isDirectory ? DDIRECTORY : DFILE, tree);
            n->size = find_data.nFileSizeLow;
            n->desc.hidden = find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN;

            sprintf(path, "%s\\%s", dir, find_data.cFileName);
            
            // If the file is a directory recurse on it
            if(isDirectory)
                n->size = dir_walk(path, n, options);

            // if(!filter(find_data.cFileName, isDirectory, find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN, options))
            if(!filter(n, options))
                continue;
                
            dirtree_insert(tree, n);

            BENCHMARK_INC_FILES(bench, isDirectory);

        } while (FindNextFile(hFind, &find_data));

        FindClose(hFind);
        
        return tree->size;
    }
#endif

void build_dirtree(dirtree *tree, const char *path, duckoptions options)
{
    clock_t begin, end;

    begin = clock();

    dir_walk(path, tree, options);

    end = clock();
    bench.dtime = (double)(end - begin) / CLOCKS_PER_SEC;

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

    end = clock();
    bench.ttime = (double)(end - begin) / CLOCKS_PER_SEC;
}