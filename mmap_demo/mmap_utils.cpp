#include "mmap_utils.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"

constexpr int32_t INVALID_INT32 = -1;

void *GetMmapBuff(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    if (length == 0) {
        PRINT_ERROR("length is invalid");
        return MAP_FAILED;
    }

    void *addr = mmap(start, length, prot, flags, fd, offset);
    return addr;
}

FileMmap *FileMmapInit(void)
{
    FileMmap *fMmap = (FileMmap *)malloc(sizeof(FileMmap));
    if (fMmap == nullptr) {
        return nullptr;
    }

    fMmap->fileFd = INVALID_INT32;
    fMmap->buff = static_cast<uint8_t *>(MAP_FAILED);
    fMmap->freeBuff = static_cast<uint8_t *>(MAP_FAILED);
    fMmap->buffSize = INVALID_INT32;
    fMmap->freeBuffSize = INVALID_INT32;

    return fMmap;
}

int SyncMmapToFile(void *buff, size_t buffSize)
{
    if ((buff != MAP_FAILED) && (buffSize > 0)) {
        auto ret = munmap(buff, buffSize);
        if (ret != 0) {
            PRINT_ERROR("failed to release file mmap.");
        }

        return ret;
    }

    return 0;
}

void FreeFileMmap(FileMmap *fMmap)
{
    if (fMmap == nullptr) {
        return;
    }

    (void)SyncMmapToFile(fMmap->buff, fMmap->buffSize);
    if (fMmap->fileFd >= 0) {
        close(fMmap->fileFd);
    }

    free(fMmap);

    return;
}

/*
 * 功能描述: 将指定文件映射为内存缓冲区
 */
FileMmap *OpenFileMmap(const char *file)
{
    /* 涉及多进程配合读写，所以，内存文件权限设置为640 */
    auto fileFd = open(file, O_RDWR | O_CREAT | O_APPEND, S_IREAD | S_IWRITE | S_IRGRP);
    if (fileFd < 0) {
        PRINT_ERROR("failed to open %s.", file);
        return nullptr;
    }

    /* 如果文件不存在或者长度为0，则暂时不做mmap映射，延后到自动扩展时再进行 */
    void *fileBuff = nullptr;
    auto fileSize = lseek(fileFd, 0, SEEK_END);
    (void)lseek(fileFd, 0, SEEK_SET);

    if (fileSize > 0) {
        fileBuff = GetMmapBuff(nullptr, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileFd, 0);
        if (fileBuff == MAP_FAILED) {
            PRINT_ERROR("get file mmap failed.");
            return nullptr;
        }
    }

    /* 生成mmap */
    FileMmap *fMmap = FileMmapInit();
    if (fMmap == nullptr) {
        PRINT_ERROR("malloc file mmap failed.");
        return nullptr;
    }

    fMmap->fileFd = fileFd;
    fMmap->freeBuff = static_cast<uint8_t *>(fileBuff);
    fMmap->buffSize = fileSize;
    fMmap->freeBuffSize = fMmap->buffSize;
    return fMmap;
}

/*
 * 功能描述: 关闭根据文件映射的内存缓冲区，并会关闭文件
 */
void CloseFileMmap(FileMmap *fMmmpRec)
{
    FreeFileMmap(fMmmpRec);
    return;
}

/*
 * 功能描述: 根据需要增加的空间大小，自动扩展映射的内存缓冲区
 */
bool EnlargeFileMmap(FileMmap *fMmmpRec, int32_t incSize)
{
    if ((fMmmpRec == nullptr) || (incSize <= 0)) {
        PRINT_ERROR("invalid fmmap(%p) or increase size(%u).", fMmmpRec, incSize);
        return false;
    }

    /* 防止扩展后的总长度超长而翻转导致出现非法值 */
    int32_t oldSize = fMmmpRec->buffSize;
    if (oldSize + incSize <= 0) {
        PRINT_ERROR("oldSize + incSize overflow.");
        return false;
    }

    /* 将旧内容落盘 */
    auto ret = SyncMmapToFile(fMmmpRec->buff, fMmmpRec->buffSize);
    if (ret < 0) {
        return false;
    }

    /* 增加文件大小 */
    ret = lseek(fMmmpRec->fileFd, incSize, SEEK_END);
    if (ret < 0) {
        PRINT_ERROR("lseek failed, incSize is: %u.", incSize);
        return false;
    }

    /* 
     * 不同于网上查到的lseek扩展文件的方式，即在文件末尾write(fMmmpRec->fileFd, "\0", 1)，linux并没有自动给其它位置充填
     * 像是在上一次lseek之后游标自动回到了文件开头一样，导致文件大小只有1byte
     * ps：已知的情况可能为：设置O_APPEND模式后，lseek到文件结束后，实际不会进行偏移 也就是无法形成空洞效果，文件实质扩展失败
     */
    lseek(fMmmpRec->fileFd, 0, oldSize);
    if (write(fMmmpRec->fileFd, "\0", incSize) < 0) {
        PRINT_ERROR("write to file mmap failed.");
        return false;
    }
    lseek(fMmmpRec->fileFd, 0, oldSize);

    auto addr = GetMmapBuff(nullptr, oldSize + incSize, PROT_READ | PROT_WRITE, MAP_SHARED, fMmmpRec->fileFd, 0);
    if (addr == MAP_FAILED) {
        PRINT_ERROR("get file mmap failed.");
        return false;
    }

    fMmmpRec->buff = static_cast<uint8_t *>(addr);
    fMmmpRec->buffSize = oldSize + incSize;
    fMmmpRec->freeBuff = fMmmpRec->buff + oldSize;
    fMmmpRec->freeBuffSize = incSize;

    return true;
}
