#ifndef MMAP_UTILS_H
#define MMAP_UTILS_H

#include <stdint.h>

struct FileMmap {
    int32_t fileFd;       /* 映射的文件句柄 */
    uint8_t *buff;        /* 总的内存地址 */
    uint8_t *freeBuff;    /* 可用的内存起始地址 */
    int32_t buffSize;     /* 文件总大小 */
    int32_t freeBuffSize; /* 可用的内存大小 */
};

#ifdef __cplusplus
extern "C" {
#endif

FileMmap *OpenFileMmap(const char *file);
void CloseFileMmap(FileMmap *fileMmap);
bool EnlargeFileMmap(FileMmap *fileMmap, int32_t incSize);

#ifdef __cplusplus
}
#endif

#endif  // MMAP_UTILS_H