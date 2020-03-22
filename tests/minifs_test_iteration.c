#define MINIFS_IMPLEMENTATION
#include "../minifs.h"

int main(int argc, char** argv)
{
    mfs_result result;
    mfs_iterator iterator;
    mfs_file_info fileInfo;

    (void)argc;
    (void)argv;

    result = mfs_iterator_init("", &iterator);
    if (result != MFS_SUCCESS) {
        printf("Failed to initialize iterator: %d\n", result);
        return (int)result;
    }

    while (mfs_iterator_next(&iterator, &fileInfo) == MFS_SUCCESS) {
        if (fileInfo.isDirectory) {
            printf("DIR:  %s\n", fileInfo.pFileName);
        } else {
            printf("FILE: %s\n", fileInfo.pFileName);
        }
    }

    /* In this test, the file testvectors/iteration/a.txt will contain the structure of our iteration test vector. */

    return 0;
}
