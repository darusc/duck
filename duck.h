#ifndef DUCK_H
#define DUCK_H

#include <stdint.h>

#ifdef __unix__
    #define MAX_PATH 260
    #include <sys/types.h>
#else
    #include <windows.h>
#endif 

enum filetype
{
    DFILE,
    DDIRECTORY
};

static const char *magnitudes = "KMGT";


typedef struct duckoptions
{
    int benchmark;
    int hide;

    enum sortmethod
    {
        DSIZE,
        DALPHABETIC,
        DITEMS
    } sort;

} duckoptions;


/**
 * Benchmark result data
 */
typedef struct benchmark
{
    double ttime; // total time
    double dtime; // discovery time
    int files;
    int directories;
} benchmark;

#define BENCHMARK_LINES 4
#define BENCHMARK_INC_FILES(bench, dir) if(dir) \
                                            bench.directories++; \
                                        else \
                                            bench.files++;

extern benchmark bench;


typedef struct dirtree_desc
{
    enum filetype type;
    char name[MAX_PATH];
    uint8_t length;
} dirtree_desc;

/**
 * Node of the directory tree
 */
typedef struct dirtree
{
    struct dirtree_desc desc;

    /**
     * Selected (highlighted) file.
     * When going down a level this will be the opened directory
     */
    uint32_t selected_file;

    /**
     * Size of the current file / directory in bytes;
     */
    size_t size;

    /**
     * Number of files / directories contained
     */
    uint32_t nfiles;
    
    uint32_t capacity;
    struct dirtree **files;

    struct dirtree *parent;
} dirtree;

dirtree* dirtree_alloc(const char* name, enum filetype type, dirtree *parent);

void dirtree_insert(dirtree *root, dirtree* node);
void dirtree_sort(dirtree *root, int (*comparator)(const void*, const void*));

/**
 * Go down one level. Open the selected directory
 */
int dirtree_down(dirtree **root);

/**
 * Go up one level. Navigate back.
 */
int dirtree_up(dirtree **root);

int dirtree_select_next_file(dirtree *root);
int dirtree_select_prev_file(dirtree *root);

/**
 * Returns the path from root for the given file (dritree node).
 * @returns The length of path
 */
int dirtree_getpath(dirtree *tree, char *path);

/**
 * Transforms the given size in bytes to kb/mb/gb 
 * to respect the following format %3.2d
 */
void size(double size, char* out);

/**
 * Recursively traverse all the files and directory starting at path
 * and build the output directory tree
 */
void build_dirtree(dirtree *tree, const char *path, duckoptions options);

#endif