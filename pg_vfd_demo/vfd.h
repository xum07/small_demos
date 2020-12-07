#ifndef VFD_H
#define VFD_H

typedef int File;

extern void InitFileAccess();
extern File PathNameOpenFile(const char *fileName);

#endif  // VFD_H