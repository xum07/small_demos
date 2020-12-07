#include <cstdlib>
#include <iostream>
#include <memory>
#include <string.h>

#include "vfd.h"
#include "utils.h"

typedef struct vfd {
    FILE *fd;
    File nextFree;        /* link to next free VFD, if in freelist */
    File lruMoreRecently; /* doubly linked recency-of-use list */
    File lruLessRecently;
    char *fileName; /* name of file, or NULL for unused VFD */
    /* NB: fileName is malloc'd, and must be free'd when closing the VFD */
} Vfd;

// use LRU cache as a fixed size
#define LRU_SIZE 2
// must bigger than LRU_SIZE to avoid vfd running out
#define VFD_CACHE_SIZE 6

static vfd *VfdCache;
static File openFiles;

static File AllocateVfd();
static void FreeVfd(File file);
static void LruDelete(File file);
static void ReleaseLruFiles();

void InitFileAccess()
{
    VfdCache = (Vfd *)malloc(VFD_CACHE_SIZE * sizeof(Vfd));
    if (VfdCache == nullptr) {
        PRINT_ERROR("out of memory.");
        exit(1);
    }

    for (int i = 0; i < VFD_CACHE_SIZE; i++) {
        memset((char *)&(VfdCache[i]), 0, sizeof(Vfd));
        VfdCache[i].fd = nullptr;
        VfdCache[i].nextFree = i + 1;
    }
    VfdCache[VFD_CACHE_SIZE - 1].nextFree = 0;
    VfdCache[0].nextFree = 1;
    openFiles = 0;
}

static File AllocateVfd()
{
    File file = VfdCache[0].nextFree;
    VfdCache[0].nextFree = VfdCache[file].nextFree;
    return file;
}

static void FreeVfd(File file)
{
    Vfd *vfdP = &VfdCache[file];
    PRINT_INFO("%s is going to release.", vfdP->fileName);

    if (vfdP->fileName != nullptr) {
        free(vfdP->fileName);
        vfdP->fileName = nullptr;
    }

    vfdP->nextFree = VfdCache[0].nextFree;
    VfdCache[0].nextFree = file;
}

static void LruDelete(File file)
{
    Vfd *vfdP = &VfdCache[file];
    if (fclose(vfdP->fd)) {
        PRINT_ERROR("can not close file: %s.", vfdP->fileName);
        exit(1);
    }

    vfdP->fd = nullptr;
    VfdCache[vfdP->lruLessRecently].lruMoreRecently = vfdP->lruMoreRecently;
    VfdCache[vfdP->lruMoreRecently].lruLessRecently = vfdP->lruLessRecently;
    FreeVfd(file);

    openFiles--;
}

File PathNameOpenFile(const char *fileName)
{
    Vfd		   *vfdP;
    File        file;

    char *fnamecopy = strdup(fileName);
    if (fnamecopy == nullptr) {
        PRINT_ERROR("out of memory");
        exit(1);
    }

    ReleaseLruFiles();

    file = AllocateVfd();
    vfdP = &VfdCache[file];

    FILE *fd = fopen(fnamecopy, "r");
    if (fd == nullptr) {
        PRINT_ERROR("can not open file: %s", fnamecopy);
        exit(1);
    }

    vfdP->fd = fd;
    vfdP->fileName = fnamecopy;
    vfdP->lruMoreRecently = 0;
    vfdP->lruLessRecently = VfdCache[0].lruLessRecently;
    VfdCache[0].lruLessRecently = file;
    VfdCache[vfdP->lruLessRecently].lruMoreRecently = file;

    openFiles++;
    return file;
}

static void ReleaseLruFiles()
{
    while(openFiles >= LRU_SIZE) {
        PRINT_INFO("LRU pool is full, need to clear file: %s.", VfdCache[VfdCache[0].lruMoreRecently].fileName);
        LruDelete(VfdCache[0].lruMoreRecently);
    }
}