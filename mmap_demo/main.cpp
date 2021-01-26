#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "mmap_utils.h"
#include "utils.h"

using std::string;

bool WriteInMmap(string content, FileMmap *dstFileMmap)
{
    if (EnlargeFileMmap(dstFileMmap, content.length()) == false) {
        PRINT_ERROR("failed to enlarge dstFileMmap to size(%u).", content.length());
        return false;
    }

    content.copy(reinterpret_cast<char *>(dstFileMmap->freeBuff), content.length());
    return true;
}

int main()
{
    string dstFile = "dst_file";
    remove(dstFile.c_str());

    auto dstFileMmap = OpenFileMmap(dstFile.c_str());
    if (dstFileMmap == nullptr) {
        PRINT_ERROR("failed to open file(%s) through mmap.", dstFile);
        return -1;
    }

    // write something to mmap
    auto result = WriteInMmap("Hello world!\n", dstFileMmap);
    if (result) {
        WriteInMmap("This is a mmap test.\n", dstFileMmap);
    }

    CloseFileMmap(dstFileMmap);
    return 0;
}