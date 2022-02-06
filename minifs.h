/*
File system library. Choice of public domain or MIT-0. See license statements at the end of this file.
minifs - v0.0.0 (unreleased) - TBD

David Reid - davidreidsoftware@gmail.com
*/
#ifndef minifs_h
#define minifs_h

#ifdef __cplusplus
extern "C" {
#endif

/* Platform detection. */
#ifdef _WIN32
    #define MFS_WIN32
    #if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP || WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
        #define MFS_WIN32_UWP
    #else
        #define MFS_WIN32_DESKTOP
    #endif
#else
    #define MFS_POSIX
    #ifdef __unix__
        #define MFS_UNIX
        #if defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
            #define MFS_BSD
        #endif
    #endif
    #ifdef __linux__
        #define MFS_LINUX
    #endif
    #ifdef __APPLE__
        #define MFS_APPLE
    #endif
    #ifdef __ANDROID__
        #define MFS_ANDROID
    #endif
    #ifdef __EMSCRIPTEN__
        #define MFS_EMSCRIPTEN
    #endif
#endif

#if defined(_MSC_VER)
    typedef struct _stat64 mfs_stat_info;
#elif defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64 && defined(_LARGEFILE64_SOURCE)
    typedef struct stat64 mfs_stat_info;
#else
    typedef struct stat mfs_stat_info;
#endif

/* Sized types. Prefer built-in types. Fall back to stdint. */
#include <stddef.h> /* For size_t. */
#ifdef _MSC_VER
    #if defined(__clang__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wlanguage-extension-token"
        #pragma GCC diagnostic ignored "-Wc++11-long-long"
    #endif
    typedef   signed __int8  mfs_int8;
    typedef unsigned __int8  mfs_uint8;
    typedef   signed __int16 mfs_int16;
    typedef unsigned __int16 mfs_uint16;
    typedef   signed __int32 mfs_int32;
    typedef unsigned __int32 mfs_uint32;
    typedef   signed __int64 mfs_int64;
    typedef unsigned __int64 mfs_uint64;
    #if defined(__clang__)
        #pragma GCC diagnostic pop
    #endif
#else
    #define MFS_HAS_STDINT
    #include <stdint.h>
    typedef int8_t   mfs_int8;
    typedef uint8_t  mfs_uint8;
    typedef int16_t  mfs_int16;
    typedef uint16_t mfs_uint16;
    typedef int32_t  mfs_int32;
    typedef uint32_t mfs_uint32;
    typedef int64_t  mfs_int64;
    typedef uint64_t mfs_uint64;
#endif

#ifdef MFS_HAS_STDINT
    typedef uintptr_t mfs_uintptr;
#else
    #if defined(_WIN32)
        #if defined(_WIN64)
            typedef mfs_uint64 mfs_uintptr;
        #else
            typedef mfs_uint32 mfs_uintptr;
        #endif
    #elif defined(__GNUC__)
        #if defined(__LP64__)
            typedef mfs_uint64 mfs_uintptr;
        #else
            typedef mfs_uint32 mfs_uintptr;
        #endif
    #else
        typedef mfs_uint64 mfs_uintptr;   /* Fallback. */
    #endif
#endif
typedef mfs_uint8   mfs_bool8;
typedef mfs_uint32  mfs_bool32;
#define MFS_TRUE    1
#define MFS_FALSE   0

typedef void* mfs_handle;
typedef void* mfs_ptr;
typedef void (* mfs_proc)(void);

/* Limits. */
#if defined(SIZE_MAX)
    #define MFS_SIZE_MAX    SIZE_MAX
#else
    #define MFS_SIZE_MAX    0xFFFFFFFF  /* When SIZE_MAX is not defined by the standard library just default to the maximum 32-bit unsigned integer. */
#endif

/* Define NULL for some compilers. */
#ifndef NULL
#define NULL 0
#endif

#include <stdio.h>


/* Standard result codes. */
typedef int mfs_result;
#define MFS_SUCCESS                           0
#define MFS_ERROR                            -1
#define MFS_INVALID_ARGS                     -2
#define MFS_INVALID_OPERATION                -3
#define MFS_OUT_OF_MEMORY                    -4
#define MFS_OUT_OF_RANGE                     -5
#define MFS_ACCESS_DENIED                    -6
#define MFS_DOES_NOT_EXIST                   -7
#define MFS_ALREADY_EXISTS                   -8
#define MFS_TOO_MANY_OPEN_FILES              -9
#define MFS_INVALID_FILE                     -10
#define MFS_TOO_BIG                          -11
#define MFS_PATH_TOO_LONG                    -12
#define MFS_NAME_TOO_LONG                    -13
#define MFS_NOT_DIRECTORY                    -14
#define MFS_IS_DIRECTORY                     -15
#define MFS_DIRECTORY_NOT_EMPTY              -16
#define MFS_END_OF_FILE                      -17
#define MFS_NO_SPACE                         -18
#define MFS_BUSY                             -19
#define MFS_IO_ERROR                         -20
#define MFS_INTERRUPT                        -21
#define MFS_UNAVAILABLE                      -22
#define MFS_ALREADY_IN_USE                   -23
#define MFS_BAD_ADDRESS                      -24
#define MFS_BAD_SEEK                         -25
#define MFS_BAD_PIPE                         -26
#define MFS_DEADLOCK                         -27
#define MFS_TOO_MANY_LINKS                   -28
#define MFS_NOT_IMPLEMENTED                  -29
#define MFS_NO_MESSAGE                       -30
#define MFS_BAD_MESSAGE                      -31
#define MFS_NO_DATA_AVAILABLE                -32
#define MFS_INVALID_DATA                     -33
#define MFS_TIMEOUT                          -34
#define MFS_NO_NETWORK                       -35
#define MFS_NOT_UNIQUE                       -36
#define MFS_NOT_SOCKET                       -37
#define MFS_NO_ADDRESS                       -38
#define MFS_BAD_PROTOCOL                     -39
#define MFS_PROTOCOL_UNAVAILABLE             -40
#define MFS_PROTOCOL_NOT_SUPPORTED           -41
#define MFS_PROTOCOL_FAMILY_NOT_SUPPORTED    -42
#define MFS_ADDRESS_FAMILY_NOT_SUPPORTED     -43
#define MFS_SOCKET_NOT_SUPPORTED             -44
#define MFS_CONNECTION_RESET                 -45
#define MFS_ALREADY_CONNECTED                -46
#define MFS_NOT_CONNECTED                    -47
#define MFS_CONNECTION_REFUSED               -48
#define MFS_NO_HOST                          -49
#define MFS_IN_PROGRESS                      -50
#define MFS_CANCELLED                        -51
#define MFS_MEMORY_ALREADY_MAPPED            -52
#define MFS_AT_END                           -53


typedef struct
{
    char pFileName[256];
    mfs_uint64 sizeInBytes;
    mfs_uint64 lastModifiedTime;
    mfs_uint64 lastAccessTime;
    mfs_bool32 isDirectory : 1;
    mfs_bool32 isReadOnly  : 1;
} mfs_file_info;


/*
File Reading
*/

/*
Opens a stdio FILE object.
*/
mfs_result mfs_fopen(FILE** ppFile, const char* pFilePath, const char* pOpenMode);
mfs_result mfs_wfopen(FILE** ppFile, const wchar_t* pFilePath, const wchar_t* pOpenMode);

/*
Closes a stdio FILE object.
*/
mfs_result mfs_fclose(FILE* pFile);

/*
Reads data from a stdio FILE object.
*/
mfs_result mfs_fread(FILE* pFile, void* pData, size_t sizeInBytes, size_t* pBytesRead);

/*
Writes data to a stdio FILE object.
*/
mfs_result mfs_fwrite(FILE* pFile, const void* pData, size_t sizeInBytes, size_t* pBytesWritten);

/*
Seeks a stdio FILE object.
*/
mfs_result mfs_fseek(FILE* pFile, mfs_int64 offset, int origin);

/*
Retrieves the read/write point of a stdio FILE object.
*/
mfs_int64 mfs_ftell(FILE* pFile);

/*
Retrieves information about a stdio FILE object.
*/
mfs_result mfs_fstat(FILE* pFile, mfs_stat_info* info);



/*
High level API for opening and reading a file.

Free the file data with mfs_free().
*/
mfs_result mfs_open_and_read_file(const char* pFilePath, size_t* pFileSizeOut, void** ppFileData);

/*
High level API for opening and reading a text file.

Free the file data with mfs_free().
*/
mfs_result mfs_open_and_read_text_file(const char* pFilePath, size_t* pFileSizeOut, char** ppFileData);



/*
High level API for opening and writing a file.
*/
mfs_result mfs_open_and_write_file(const char* pFilePath, size_t fileSize, const void* pFileData);



/*
Directory Management
*/

/*
Sets the current directory.
*/
mfs_result mfs_set_current_directory(const char* pDirectoryPath);

/*
Retrieves the current directory.

If the output buffer is not large enough to store the entire result, it will be set to an empty string and MFS_OUT_OF_RANGE
will be returned.
*/
mfs_result mfs_get_current_directory(char* pDirectoryPath, size_t directoryPathSizeInBytes);

/*
Creates a directory.
*/
mfs_result mfs_mkdir(const char* pDirectory, mfs_bool32 recursive);

/*
Deletes a directory.
*/
mfs_result mfs_rmdir(const char* pDirectory, mfs_bool32 recursive);

/*
Recursively deletes the contents of a directory.
*/
mfs_result mfs_rmdir_content(const char* pDirectory);


/*
Checks if the given path refers to an existing directory.
*/
mfs_bool32 mfs_is_directory(const char* pPath);

/*
Checks if a file exists.
*/
mfs_bool32 mfs_file_exists(const char* pFilePath);


/*
Copies a file.
*/
mfs_result mfs_copy_file(const char* pSrcFilePath, const char* pDstFilePath, mfs_bool32 failIfExists);

/*
Moves a file.
*/
mfs_result mfs_move_file(const char* pSrcFilePath, const char* pDstFilePath, mfs_bool32 failIfExists);

/*
Deletes a file or folder.
*/
mfs_result mfs_delete_file(const char* pFilePath);


/*
Retrieves basic information about a file.

Returns MFS_SUCCESS on success. Otherwise returns an error code.
*/
mfs_result mfs_get_file_info(const char* pFilePath, mfs_file_info* pFileInfo);



/* Iteration */
typedef struct
{
#if defined(_WIN32)
    struct
    {
        mfs_handle handle;
        mfs_file_info fi;   /* <-- For dancing around differences between the minifs API and the Win32 API. */
        mfs_bool32 atEnd;
    } win32;
#else
    struct
    {
        /*DIR**/ void* dir;
        char* pPath;
    } posix;
#endif
} mfs_iterator;

/*
Initializes an iterator.

Iterators are used to iterate over each of the files in a directory.
*/
mfs_result mfs_iterator_init(const char* pDirectoryPath, mfs_iterator* pIterator);

/*
Uninitializes an iterator.
*/
void mfs_iterator_uninit(mfs_iterator* pIterator);

/*
Gets the next file in the iteration.

Returns MFS_SUCCESS on success. Returns MFS_AT_END if it's reached the end of iteration. Otherwise an error code is returned.
*/
mfs_result mfs_iterator_next(mfs_iterator* pIterator, mfs_file_info* pFileInfo);


/*
Paths
=====
The path API is basically just a string library where the input strings are just paths. These APIs follow the following convensions:
  * All APIs generally follow the safe string API:
    * Return values are error codes.
    * First parameter is always the destination, second parameter is always the destination capacity and the third parameter is
      always the source string.
    * Passing in NULL for the source is an error as this would normally indicate an underlying error.
    * Unlike the standard safe string APIs, the destination can be NULL in which case the API can be used to measure the output string.
  * All APIs copy the string, unless they're named as "_in_place".
  * All APIs that make a copy of the source string will have an optional parameter for receiving the length of the destination string.
*/
typedef struct
{
    size_t offset;
    size_t length;
} mfs_path_segment;

/*
Checks whether or not two path segments are equal.
*/
mfs_bool32 mfs_path_segments_equal(const char* s0Path, const mfs_path_segment s0, const char* s1Path, const mfs_path_segment s1);


typedef struct
{
    const char* path;
    mfs_path_segment segment;
} mfs_path_iterator;

/*
Initializes a path iterator, starting from the first segment.
*/
mfs_result mfs_path_first_segment(const char* path, mfs_path_iterator* pIterator);

/*
Initializes a path iterator, starting from the end segment.
*/
mfs_result mfs_path_last_segment(const char* path, mfs_path_iterator* pIterator);

/*
Moves a path iterator to the next segment.
*/
mfs_result mfs_path_next_segment(mfs_path_iterator* pIterator);

/*
Moves a path iterator to the previous segment.
*/
mfs_result mfs_path_prev_segment(mfs_path_iterator* pIterator);

/*
Determines if a path iterator is at the start.
*/
mfs_bool32 mfs_path_at_start(mfs_path_iterator iterator);

/*
Determines whether or not the iterator is at the end.
*/
mfs_bool32 mfs_path_at_end(mfs_path_iterator iterator);

/*
Checks if two iterators are equal.
*/
mfs_bool32 mfs_path_iterators_equal(const mfs_path_iterator i0, const mfs_path_iterator i1);


/*
Determines whether or not the given path refers to the root segment of a path.
*/
mfs_bool32 mfs_path_is_root(const char* path);
mfs_bool32 mfs_path_is_root_segment(const char* path, const mfs_path_segment segment);

/*
Determines whether or not the given path refers to a Unix style root directory ("/")
*/
mfs_bool32 mfs_path_is_unix_style_root(const char* path);
mfs_bool32 mfs_path_is_unix_style_root_segment(const char* path, const mfs_path_segment segment);

/*
Determines whether or not the given path refers to a Windows style root directory.
*/
mfs_bool32 mfs_path_is_win32_style_root(const char* path);
mfs_bool32 mfs_path_is_win32_style_root_segment(const char* path, const mfs_path_segment segment);


/*
Copies a path.

This is basically the same as strcpy_s(), except the length of the string is returned in [pDstLenOut].
*/
mfs_result mfs_path_copy(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut);


/*
Copies a path and normalizes the slashes to forward slashes.
*/
mfs_result mfs_path_to_forward_slashes(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut);

/*
Copies a path and normalizes the slashes to back slashes.
*/
mfs_result mfs_path_to_back_slashes(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut);

/*
Determines whether or not the given path is a decendant of another.

descendantAbsolutePath [in] The absolute path of the descendant.
parentAbsolutePath     [in] The absolute path of the parent.

As an example, "C:/My/Folder" is a descendant of "C:/".

If either path contains "." or "..", clean it with mfs_path_clean() before calling this.
*/
mfs_bool32 mfs_path_is_descendant(const char* descendantAbsolutePath, const char* parentAbsolutePath);

/*
Determines whether or not the given path is a direct child of another.

childAbsolutePath [in] The absolute of the child.
parentAbsolutePath [in] The absolute path of the parent.

As an example, "C:/My/Folder" is NOT a child of "C:/" - it is a descendant. Alternatively, "C:/My" IS a child of "C:/".

If either path contains "." or "..", clean it with mfs_path_clean() before calling this.
*/
mfs_bool32 mfs_path_is_child(const char* childAbsolutePath, const char* parentAbsolutePath);


/*
Finds the file name portion of the path.

path [in] The path to search.

Returns a pointer to the beginning of the string containing the file name. If this is non-null, it will always be an offset of "path".

A path with a trailing slash will return an empty string.
*/
const char* mfs_path_file_name(const char* path);

/*
Finds the file extension of the given file path.

path [in] The path to search.

Returns a pointer to the beginning of the string containing the path's extension. If the path does not have an extension, an empty
string will be returned. The return value is always an offset of "path".

A path with a trailing slash will return an empty string.

On a path such as "filename.ext1.ext2" the returned string will be "ext2".
*/
const char* mfs_path_extension(const char* path);

/*
Determines whether or not the given path has an extension.
*/
mfs_bool32 mfs_path_has_extension(const char* path);


/*
Checks whether or not the two paths are equal.

path1 [in] The first path.
path2 [in] The second path.

Returns whether or not the operation is successful.

This is case-sensitive.

This is more than just a string comparison. Rather, this splits the path and compares each segment. The path "C:/My/Folder" is considered
equal to to "C:\\My\\Folder".
*/
mfs_bool32 mfs_path_equal(const char* path1, const char* path2);

/*
Checks if the extension of the given path is equal to the given extension.

This is not case sensitive. If you want this to be case sensitive, just do "strcmp(mfs_path_extension(path), extension)".
*/
mfs_bool32 mfs_path_extension_equal(const char* path, const char* extension);


/*
Determines whether or not the given path is relative.

path [in] The path to check.
*/
mfs_bool32 mfs_path_is_relative(const char* path);

/*
Determines whether or not the given path is absolute.

path [in] The path to check.
*/
mfs_bool32 mfs_path_is_absolute(const char* path);


/*
Retrieves the base path from the given path, not including the trailing slash.

dst            [out] The destination buffer.
dstSizeInBytes [in]  The size of the buffer pointed to by "pathOut", in bytes.
src            [in]  The input path.

Returns whether or not the operation is successful.

As an example, when "src" is "C:/MyFolder/MyFile", the output will be "C:/MyFolder". Note that there is no trailing slash.

If "src" is something like "/MyFolder", the return value will be an empty string.
*/
mfs_result mfs_path_base_path(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut);

/*
Retrieves the file name portion of the given path, without the extension.

dst            [out] The destination buffer.
dstSizeInBytes [in]  The size of the buffer pointed to by "pathOut", in bytes.
src            [in]  The input path.
*/
mfs_result mfs_path_file_name_without_extension(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut);


/*
Appends two paths together, and copies them to a separate buffer.

dst            [out] The destination buffer.
dstSizeInBytes [in]  The size of the buffer pointed to by "pathOut", in bytes.
base           [in]  The base directory.
other          [in]  The relative path to append to "base".

Returns whether or not the operation is successful.

This assumes both paths are well formed and "other" is a relative path.

"pathOut" and "base" are allowed to be the same pointer, in which case the other path is appended in-place.
*/
mfs_result mfs_path_append(char* dst, size_t dstSizeInBytes, const char* base, const char* other, size_t* pDstLenOut);

/*
Appends a base path and an iterator together, and copyies them to a separate buffer.

dst            [out] The destination buffer.
dstSizeInBytes [in]  The size of the buffer pointed to by "dst", in bytes.
base           [in]  The base directory.
iterator       [in]  The iterator to append.

Returns whether or not the operation is successful.

This assumes both paths are well formed and "i" is a valid iterator.

"pathOut" and "base" are allowed to be the same pointer, in which case the other path is appended in-place.
*/
mfs_result mfs_path_append_iterator(char* dst, size_t dstSizeInBytes, const char* base, mfs_path_iterator iterator, size_t* pDstLenOut);

/*
Appends an extension to the given base path and copies them to a separate buffer.

dst            [out] The destination buffer.
dstSizeInBytes [in]  The size of the buffer pointed to by "dst", in bytes.
base           [in]  The base directory.
extension      [in]  The relative path to append to "base".

Returns whether or not the operation is successful.

"pathOut" and "base" are allowed to be the same pointer, in which case the other path is appended in-place.
*/
mfs_result mfs_path_append_extension(char* dst, size_t dstSizeInBytes, const char* base, const char* extension, size_t* pDstLenOut);


/*
Cleans the path and resolves the ".." and "." segments.

dst            [out] A pointer to the buffer that will receive the path.
dstSizeInBytes [in]  The size of the buffer pointed to by pathOut, in bytes.
src            [in]  The path to clean.

Returns whether or not the operation is successful.

The output path will never be longer than the input path.

The output buffer should never overlap with the input path.

As an example, the path "my/messy/../path" will result in "my/path"

The path "my/messy/../../../path" (note how there are too many ".." segments) will return "path" (the extra ".." segments will be dropped.)

If an error occurs, such as an invalid input path, 0 will be returned.
*/
mfs_result mfs_path_clean(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut);

/*
Appends one path to the other and then cleans it.
*/
mfs_result mfs_path_append_and_clean(char* dst, size_t dstSizeInBytes, const char* base, const char* other, size_t* pDstLenOut);


/*
Removes the extension from a path.

If the path does not have an extension, the source string is copied straight into the destination without any changes and [pDstLenOut]
receives the length of the string.
*/
mfs_result mfs_path_remove_extension(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut);
mfs_result mfs_path_remove_extension_in_place(char* path, size_t* pDstLenOut);

/*
Creates a copy of the given string and removes the extension.
*/
mfs_result mfs_path_remove_file_name(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut);
mfs_result mfs_path_remove_file_name_in_place(char* path, size_t* pDstLenOut);


/*
Converts an absolute path to a relative path.

Returns whether or not the operation is successful.

This will normalize every slash to forward slashes.
*/
mfs_result mfs_path_to_relative(char* dst, size_t dstSizeInBytes, const char* absolutePathToMakeRelative, const char* absolutePathToMakeRelativeTo, size_t* pDstLenOut);

/*
Converts a relative path to an absolute path based on a base path.

Returns true if the conversion was successful; false if there was an error.

This is equivalent to an append followed by a clean. Slashes will be normalized to forward slashes.
*/
mfs_result mfs_path_to_absolute(char* dst, size_t dstSizeInBytes, const char* relativePathToMakeAbsolute, const char* basePath, size_t* pDstLenOut);


/*
Miscellaneous APIs.
*/

/*
Frees memory that was previously allocated by a public API, such as mfs_open_and_read_file().
*/
void mfs_free(void* p);


#ifdef __cplusplus
}
#endif
#endif  /* minifs_h */


/************************************************************************************************************************************************************
*************************************************************************************************************************************************************

IMPLEMENTATION

*************************************************************************************************************************************************************
************************************************************************************************************************************************************/
#if defined(MINIFS_IMPLEMENTATION) || defined(MFS_IMPLEMENTATION)
#ifndef minifs_c
#define minifs_c

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <wchar.h>

#ifndef MFS_MALLOC
#ifdef MFS_WIN32
#define MFS_MALLOC(sz) HeapAlloc(GetProcessHeap(), 0, (sz))
#else
#define MFS_MALLOC(sz) malloc((sz))
#endif
#endif

#ifndef MFS_REALLOC
#ifdef MFS_WIN32
#define MFS_REALLOC(p, sz) (((sz) > 0) ? ((p) ? HeapReAlloc(GetProcessHeap(), 0, (p), (sz)) : HeapAlloc(GetProcessHeap(), 0, (sz))) : ((VOID*)(size_t)(HeapFree(GetProcessHeap(), 0, (p)) & 0)))
#else
#define MFS_REALLOC(p, sz) realloc((p), (sz))
#endif
#endif

#ifndef MFS_FREE
#ifdef MFS_WIN32
#define MFS_FREE(p) HeapFree(GetProcessHeap(), 0, (p))
#else
#define MFS_FREE(p) free((p))
#endif
#endif

#ifndef MFS_ZERO_MEMORY
#ifdef MFS_WIN32
#define MFS_ZERO_MEMORY(p, sz) ZeroMemory((p), (sz))
#else
#define MFS_ZERO_MEMORY(p, sz) memset((p), 0, (sz))
#endif
#endif

#ifndef MFS_COPY_MEMORY
#ifdef MFS_WIN32
#define MFS_COPY_MEMORY(dst, src, sz) CopyMemory((dst), (src), (sz))
#else
#define MFS_COPY_MEMORY(dst, src, sz) memcpy((dst), (src), (sz))
#endif
#endif

#ifndef MFS_ASSERT
#ifdef MFS_WIN32
#define MFS_ASSERT(condition) assert(condition)
#else
#define MFS_ASSERT(condition) assert(condition)
#endif
#endif

#define MFS_ZERO_OBJECT(p) MFS_ZERO_MEMORY((p), sizeof(*(p)))

#if defined(MFS_WIN32)
#include <windows.h>

mfs_result mfs_result_from_GetLastError(DWORD error)
{
    switch (error)
    {
        case ERROR_SUCCESS:             return MFS_SUCCESS;
        case ERROR_PATH_NOT_FOUND:      return MFS_DOES_NOT_EXIST;
        case ERROR_TOO_MANY_OPEN_FILES: return MFS_TOO_MANY_OPEN_FILES;
        case ERROR_NOT_ENOUGH_MEMORY:   return MFS_OUT_OF_MEMORY;
        case ERROR_DISK_FULL:           return MFS_NO_SPACE;
        case ERROR_HANDLE_EOF:          return MFS_END_OF_FILE;
        case ERROR_NEGATIVE_SEEK:       return MFS_BAD_SEEK;
        case ERROR_INVALID_PARAMETER:   return MFS_INVALID_ARGS;
        case ERROR_ACCESS_DENIED:       return MFS_ACCESS_DENIED;
        case ERROR_SEM_TIMEOUT:         return MFS_TIMEOUT;
        case ERROR_FILE_NOT_FOUND:      return MFS_DOES_NOT_EXIST;
        default: break;
    }

    return MFS_ERROR;
}
#endif
#if defined(MFS_POSIX)
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h> /* For open() flags. */
#include <strings.h>    /* For strcasecmp(). */
#endif

const char* mfs_result_description(mfs_result result)
{
    switch (result)
    {
        case MFS_SUCCESS:                       return "No error";
        case MFS_ERROR:                         return "Unknown error";
        case MFS_INVALID_ARGS:                  return "Invalid argument";
        case MFS_INVALID_OPERATION:             return "Invalid operation";
        case MFS_OUT_OF_MEMORY:                 return "Out of memory";
        case MFS_OUT_OF_RANGE:                  return "Out of range";
        case MFS_ACCESS_DENIED:                 return "Permission denied";
        case MFS_DOES_NOT_EXIST:                return "Resource does not exist";
        case MFS_ALREADY_EXISTS:                return "Resource already exists";
        case MFS_TOO_MANY_OPEN_FILES:           return "Too many open files";
        case MFS_INVALID_FILE:                  return "Invalid file";
        case MFS_TOO_BIG:                       return "Too large";
        case MFS_PATH_TOO_LONG:                 return "Path too long";
        case MFS_NAME_TOO_LONG:                 return "Name too long";
        case MFS_NOT_DIRECTORY:                 return "Not a directory";
        case MFS_IS_DIRECTORY:                  return "Is a directory";
        case MFS_DIRECTORY_NOT_EMPTY:           return "Directory not empty";
        case MFS_END_OF_FILE:                   return "End of file";
        case MFS_NO_SPACE:                      return "No space available";
        case MFS_BUSY:                          return "Device or resource busy";
        case MFS_IO_ERROR:                      return "Input/output error";
        case MFS_INTERRUPT:                     return "Interrupted";
        case MFS_UNAVAILABLE:                   return "Resource unavailable";
        case MFS_ALREADY_IN_USE:                return "Resource already in use";
        case MFS_BAD_ADDRESS:                   return "Bad address";
        case MFS_BAD_SEEK:                      return "Illegal seek";
        case MFS_BAD_PIPE:                      return "Broken pipe";
        case MFS_DEADLOCK:                      return "Deadlock";
        case MFS_TOO_MANY_LINKS:                return "Too many links";
        case MFS_NOT_IMPLEMENTED:               return "Not implemented";
        case MFS_NO_MESSAGE:                    return "No message of desired type";
        case MFS_BAD_MESSAGE:                   return "Invalid message";
        case MFS_NO_DATA_AVAILABLE:             return "No data available";
        case MFS_INVALID_DATA:                  return "Invalid data";
        case MFS_TIMEOUT:                       return "Timeout";
        case MFS_NO_NETWORK:                    return "Network unavailable";
        case MFS_NOT_UNIQUE:                    return "Not unique";
        case MFS_NOT_SOCKET:                    return "Socket operation on non-socket";
        case MFS_NO_ADDRESS:                    return "Destination address required";
        case MFS_BAD_PROTOCOL:                  return "Protocol wrong type for socket";
        case MFS_PROTOCOL_UNAVAILABLE:          return "Protocol not available";
        case MFS_PROTOCOL_NOT_SUPPORTED:        return "Protocol not supported";
        case MFS_PROTOCOL_FAMILY_NOT_SUPPORTED: return "Protocol family not supported";
        case MFS_ADDRESS_FAMILY_NOT_SUPPORTED:  return "Address family not supported";
        case MFS_SOCKET_NOT_SUPPORTED:          return "Socket type not supported";
        case MFS_CONNECTION_RESET:              return "Connection reset";
        case MFS_ALREADY_CONNECTED:             return "Already connected";
        case MFS_NOT_CONNECTED:                 return "Not connected";
        case MFS_CONNECTION_REFUSED:            return "Connection refused";
        case MFS_NO_HOST:                       return "No host";
        case MFS_IN_PROGRESS:                   return "Operation in progress";
        case MFS_CANCELLED:                     return "Operation cancelled";
        case MFS_MEMORY_ALREADY_MAPPED:         return "Memory already mapped";
        case MFS_AT_END:                        return "Reached end of collection";
        default:                                return "Unknown error";
    }
}

mfs_result mfs_result_from_errno(int e)
{
    switch (e)
    {
        case 0: return MFS_SUCCESS;
    #ifdef EPERM
        case EPERM: return MFS_INVALID_OPERATION;
    #endif
    #ifdef ENOENT
        case ENOENT: return MFS_DOES_NOT_EXIST;
    #endif
    #ifdef ESRCH
        case ESRCH: return MFS_DOES_NOT_EXIST;
    #endif
    #ifdef EINTR
        case EINTR: return MFS_INTERRUPT;
    #endif
    #ifdef EIO
        case EIO: return MFS_IO_ERROR;
    #endif
    #ifdef ENXIO
        case ENXIO: return MFS_DOES_NOT_EXIST;
    #endif
    #ifdef E2BIG
        case E2BIG: return MFS_INVALID_ARGS;
    #endif
    #ifdef ENOEXEC
        case ENOEXEC: return MFS_INVALID_FILE;
    #endif
    #ifdef EBADF
        case EBADF: return MFS_INVALID_FILE;
    #endif
    #ifdef ECHILD
        case ECHILD: return MFS_ERROR;
    #endif
    #ifdef EAGAIN
        case EAGAIN: return MFS_UNAVAILABLE;
    #endif
    #ifdef ENOMEM
        case ENOMEM: return MFS_OUT_OF_MEMORY;
    #endif
    #ifdef EACCES
        case EACCES: return MFS_ACCESS_DENIED;
    #endif
    #ifdef EFAULT
        case EFAULT: return MFS_BAD_ADDRESS;
    #endif
    #ifdef ENOTBLK
        case ENOTBLK: return MFS_ERROR;
    #endif
    #ifdef EBUSY
        case EBUSY: return MFS_BUSY;
    #endif
    #ifdef EEXIST
        case EEXIST: return MFS_ALREADY_EXISTS;
    #endif
    #ifdef EXDEV
        case EXDEV: return MFS_ERROR;
    #endif
    #ifdef ENODEV
        case ENODEV: return MFS_DOES_NOT_EXIST;
    #endif
    #ifdef ENOTDIR
        case ENOTDIR: return MFS_NOT_DIRECTORY;
    #endif
    #ifdef EISDIR
        case EISDIR: return MFS_IS_DIRECTORY;
    #endif
    #ifdef EINVAL
        case EINVAL: return MFS_INVALID_ARGS;
    #endif
    #ifdef ENFILE
        case ENFILE: return MFS_TOO_MANY_OPEN_FILES;
    #endif
    #ifdef EMFILE
        case EMFILE: return MFS_TOO_MANY_OPEN_FILES;
    #endif
    #ifdef ENOTTY
        case ENOTTY: return MFS_INVALID_OPERATION;
    #endif
    #ifdef ETXTBSY
        case ETXTBSY: return MFS_BUSY;
    #endif
    #ifdef EFBIG
        case EFBIG: return MFS_TOO_BIG;
    #endif
    #ifdef ENOSPC
        case ENOSPC: return MFS_NO_SPACE;
    #endif
    #ifdef ESPIPE
        case ESPIPE: return MFS_BAD_SEEK;
    #endif
    #ifdef EROFS
        case EROFS: return MFS_ACCESS_DENIED;
    #endif
    #ifdef EMLINK
        case EMLINK: return MFS_TOO_MANY_LINKS;
    #endif
    #ifdef EPIPE
        case EPIPE: return MFS_BAD_PIPE;
    #endif
    #ifdef EDOM
        case EDOM: return MFS_OUT_OF_RANGE;
    #endif
    #ifdef ERANGE
        case ERANGE: return MFS_OUT_OF_RANGE;
    #endif
    #ifdef EDEADLK
        case EDEADLK: return MFS_DEADLOCK;
    #endif
    #ifdef ENAMETOOLONG
        case ENAMETOOLONG: return MFS_PATH_TOO_LONG;
    #endif
    #ifdef ENOLCK
        case ENOLCK: return MFS_ERROR;
    #endif
    #ifdef ENOSYS
        case ENOSYS: return MFS_NOT_IMPLEMENTED;
    #endif
    #ifdef ENOTEMPTY
        case ENOTEMPTY: return MFS_DIRECTORY_NOT_EMPTY;
    #endif
    #ifdef ELOOP
        case ELOOP: return MFS_TOO_MANY_LINKS;
    #endif
    #ifdef ENOMSG
        case ENOMSG: return MFS_NO_MESSAGE;
    #endif
    #ifdef EIDRM
        case EIDRM: return MFS_ERROR;
    #endif
    #ifdef ECHRNG
        case ECHRNG: return MFS_ERROR;
    #endif
    #ifdef EL2NSYNC
        case EL2NSYNC: return MFS_ERROR;
    #endif
    #ifdef EL3HLT
        case EL3HLT: return MFS_ERROR;
    #endif
    #ifdef EL3RST
        case EL3RST: return MFS_ERROR;
    #endif
    #ifdef ELNRNG
        case ELNRNG: return MFS_OUT_OF_RANGE;
    #endif
    #ifdef EUNATCH
        case EUNATCH: return MFS_ERROR;
    #endif
    #ifdef ENOCSI
        case ENOCSI: return MFS_ERROR;
    #endif
    #ifdef EL2HLT
        case EL2HLT: return MFS_ERROR;
    #endif
    #ifdef EBADE
        case EBADE: return MFS_ERROR;
    #endif
    #ifdef EBADR
        case EBADR: return MFS_ERROR;
    #endif
    #ifdef EXFULL
        case EXFULL: return MFS_ERROR;
    #endif
    #ifdef ENOANO
        case ENOANO: return MFS_ERROR;
    #endif
    #ifdef EBADRQC
        case EBADRQC: return MFS_ERROR;
    #endif
    #ifdef EBADSLT
        case EBADSLT: return MFS_ERROR;
    #endif
    #ifdef EBFONT
        case EBFONT: return MFS_INVALID_FILE;
    #endif
    #ifdef ENOSTR
        case ENOSTR: return MFS_ERROR;
    #endif
    #ifdef ENODATA
        case ENODATA: return MFS_NO_DATA_AVAILABLE;
    #endif
    #ifdef ETIME
        case ETIME: return MFS_TIMEOUT;
    #endif
    #ifdef ENOSR
        case ENOSR: return MFS_NO_DATA_AVAILABLE;
    #endif
    #ifdef ENONET
        case ENONET: return MFS_NO_NETWORK;
    #endif
    #ifdef ENOPKG
        case ENOPKG: return MFS_ERROR;
    #endif
    #ifdef EREMOTE
        case EREMOTE: return MFS_ERROR;
    #endif
    #ifdef ENOLINK
        case ENOLINK: return MFS_ERROR;
    #endif
    #ifdef EADV
        case EADV: return MFS_ERROR;
    #endif
    #ifdef ESRMNT
        case ESRMNT: return MFS_ERROR;
    #endif
    #ifdef ECOMM
        case ECOMM: return MFS_ERROR;
    #endif
    #ifdef EPROTO
        case EPROTO: return MFS_ERROR;
    #endif
    #ifdef EMULTIHOP
        case EMULTIHOP: return MFS_ERROR;
    #endif
    #ifdef EDOTDOT
        case EDOTDOT: return MFS_ERROR;
    #endif
    #ifdef EBADMSG
        case EBADMSG: return MFS_BAD_MESSAGE;
    #endif
    #ifdef EOVERFLOW
        case EOVERFLOW: return MFS_TOO_BIG;
    #endif
    #ifdef ENOTUNIQ
        case ENOTUNIQ: return MFS_NOT_UNIQUE;
    #endif
    #ifdef EBADFD
        case EBADFD: return MFS_ERROR;
    #endif
    #ifdef EREMCHG
        case EREMCHG: return MFS_ERROR;
    #endif
    #ifdef ELIBACC
        case ELIBACC: return MFS_ACCESS_DENIED;
    #endif
    #ifdef ELIBBAD
        case ELIBBAD: return MFS_INVALID_FILE;
    #endif
    #ifdef ELIBSCN
        case ELIBSCN: return MFS_INVALID_FILE;
    #endif
    #ifdef ELIBMAX
        case ELIBMAX: return MFS_ERROR;
    #endif
    #ifdef ELIBEXEC
        case ELIBEXEC: return MFS_ERROR;
    #endif
    #ifdef EILSEQ
        case EILSEQ: return MFS_INVALID_DATA;
    #endif
    #ifdef ERESTART
        case ERESTART: return MFS_ERROR;
    #endif
    #ifdef ESTRPIPE
        case ESTRPIPE: return MFS_ERROR;
    #endif
    #ifdef EUSERS
        case EUSERS: return MFS_ERROR;
    #endif
    #ifdef ENOTSOCK
        case ENOTSOCK: return MFS_NOT_SOCKET;
    #endif
    #ifdef EDESTADDRREQ
        case EDESTADDRREQ: return MFS_NO_ADDRESS;
    #endif
    #ifdef EMSGSIZE
        case EMSGSIZE: return MFS_TOO_BIG;
    #endif
    #ifdef EPROTOTYPE
        case EPROTOTYPE: return MFS_BAD_PROTOCOL;
    #endif
    #ifdef ENOPROTOOPT
        case ENOPROTOOPT: return MFS_PROTOCOL_UNAVAILABLE;
    #endif
    #ifdef EPROTONOSUPPORT
        case EPROTONOSUPPORT: return MFS_PROTOCOL_NOT_SUPPORTED;
    #endif
    #ifdef ESOCKTNOSUPPORT
        case ESOCKTNOSUPPORT: return MFS_SOCKET_NOT_SUPPORTED;
    #endif
    #ifdef EOPNOTSUPP
        case EOPNOTSUPP: return MFS_INVALID_OPERATION;
    #endif
    #ifdef EPFNOSUPPORT
        case EPFNOSUPPORT: return MFS_PROTOCOL_FAMILY_NOT_SUPPORTED;
    #endif
    #ifdef EAFNOSUPPORT
        case EAFNOSUPPORT: return MFS_ADDRESS_FAMILY_NOT_SUPPORTED;
    #endif
    #ifdef EADDRINUSE
        case EADDRINUSE: return MFS_ALREADY_IN_USE;
    #endif
    #ifdef EADDRNOTAVAIL
        case EADDRNOTAVAIL: return MFS_ERROR;
    #endif
    #ifdef ENETDOWN
        case ENETDOWN: return MFS_NO_NETWORK;
    #endif
    #ifdef ENETUNREACH
        case ENETUNREACH: return MFS_NO_NETWORK;
    #endif
    #ifdef ENETRESET
        case ENETRESET: return MFS_NO_NETWORK;
    #endif
    #ifdef ECONNABORTED
        case ECONNABORTED: return MFS_NO_NETWORK;
    #endif
    #ifdef ECONNRESET
        case ECONNRESET: return MFS_CONNECTION_RESET;
    #endif
    #ifdef ENOBUFS
        case ENOBUFS: return MFS_NO_SPACE;
    #endif
    #ifdef EISCONN
        case EISCONN: return MFS_ALREADY_CONNECTED;
    #endif
    #ifdef ENOTCONN
        case ENOTCONN: return MFS_NOT_CONNECTED;
    #endif
    #ifdef ESHUTDOWN
        case ESHUTDOWN: return MFS_ERROR;
    #endif
    #ifdef ETOOMANYREFS
        case ETOOMANYREFS: return MFS_ERROR;
    #endif
    #ifdef ETIMEDOUT
        case ETIMEDOUT: return MFS_TIMEOUT;
    #endif
    #ifdef ECONNREFUSED
        case ECONNREFUSED: return MFS_CONNECTION_REFUSED;
    #endif
    #ifdef EHOSTDOWN
        case EHOSTDOWN: return MFS_NO_HOST;
    #endif
    #ifdef EHOSTUNREACH
        case EHOSTUNREACH: return MFS_NO_HOST;
    #endif
    #ifdef EALREADY
        case EALREADY: return MFS_IN_PROGRESS;
    #endif
    #ifdef EINPROGRESS
        case EINPROGRESS: return MFS_IN_PROGRESS;
    #endif
    #ifdef ESTALE
        case ESTALE: return MFS_INVALID_FILE;
    #endif
    #ifdef EUCLEAN
        case EUCLEAN: return MFS_ERROR;
    #endif
    #ifdef ENOTNAM
        case ENOTNAM: return MFS_ERROR;
    #endif
    #ifdef ENAVAIL
        case ENAVAIL: return MFS_ERROR;
    #endif
    #ifdef EISNAM
        case EISNAM: return MFS_ERROR;
    #endif
    #ifdef EREMOTEIO
        case EREMOTEIO: return MFS_IO_ERROR;
    #endif
    #ifdef EDQUOT
        case EDQUOT: return MFS_NO_SPACE;
    #endif
    #ifdef ENOMEDIUM
        case ENOMEDIUM: return MFS_DOES_NOT_EXIST;
    #endif
    #ifdef EMEDIUMTYPE
        case EMEDIUMTYPE: return MFS_ERROR;
    #endif
    #ifdef ECANCELED
        case ECANCELED: return MFS_CANCELLED;
    #endif
    #ifdef ENOKEY
        case ENOKEY: return MFS_ERROR;
    #endif
    #ifdef EKEYEXPIRED
        case EKEYEXPIRED: return MFS_ERROR;
    #endif
    #ifdef EKEYREVOKED
        case EKEYREVOKED: return MFS_ERROR;
    #endif
    #ifdef EKEYREJECTED
        case EKEYREJECTED: return MFS_ERROR;
    #endif
    #ifdef EOWNERDEAD
        case EOWNERDEAD: return MFS_ERROR;
    #endif
    #ifdef ENOTRECOVERABLE
        case ENOTRECOVERABLE: return MFS_ERROR;
    #endif
    #ifdef ERFKILL
        case ERFKILL: return MFS_ERROR;
    #endif
    #ifdef EHWPOISON
        case EHWPOISON: return MFS_ERROR;
    #endif
        default: return MFS_ERROR;
    }
}


/*
Return Values:
  0:  Success
  22: EINVAL
  34: ERANGE

Not using symbolic constants for errors because I want to avoid #including errno.h
*/
int mfs_strcpy_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    size_t i;

    if (dst == 0) {
        return 22;
    }
    if (dstSizeInBytes == 0) {
        return 34;
    }
    if (src == 0) {
        dst[0] = '\0';
        return 22;
    }

    for (i = 0; i < dstSizeInBytes && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (i < dstSizeInBytes) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return 34;
}

int mfs_strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
    size_t maxcount;
    size_t i;

    if (dst == 0) {
        return 22;
    }
    if (dstSizeInBytes == 0) {
        return 34;
    }
    if (src == 0) {
        dst[0] = '\0';
        return 22;
    }

    maxcount = count;
    if (count == ((size_t)-1) || count >= dstSizeInBytes) {        /* -1 = _TRUNCATE */
        maxcount = dstSizeInBytes - 1;
    }

    for (i = 0; i < maxcount && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (src[i] == '\0' || i == count || count == ((size_t)-1)) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return 34;
}

int mfs_strcat_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    char* dstorig;

    if (dst == 0) {
        return 22;
    }
    if (dstSizeInBytes == 0) {
        return 34;
    }
    if (src == 0) {
        dst[0] = '\0';
        return 22;
    }

    dstorig = dst;

    while (dstSizeInBytes > 0 && dst[0] != '\0') {
        dst += 1;
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes == 0) {
        return 22;  /* Unterminated. */
    }


    while (dstSizeInBytes > 0 && src[0] != '\0') {
        *dst++ = *src++;
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes > 0) {
        dst[0] = '\0';
    } else {
        dstorig[0] = '\0';
        return 34;
    }

    return 0;
}

mfs_bool32 mfs_string_is_null_or_empty(const char* str)
{
    return str == NULL || str[0] == '\0';
}



mfs_result mfs_fopen(FILE** ppFile, const char* pFilePath, const char* pOpenMode)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    errno_t err;
#endif

    if (ppFile != NULL) {
        *ppFile = NULL;  /* Safety. */
    }

    if (pFilePath == NULL || pOpenMode == NULL || ppFile == NULL) {
        return MFS_INVALID_ARGS;
    }

#if defined(_MSC_VER) && _MSC_VER >= 1400
    err = fopen_s(ppFile, pFilePath, pOpenMode);
    if (err != 0) {
        return mfs_result_from_errno(err);
    }
#else
#if defined(_WIN32) || defined(__APPLE__)
    *ppFile = fopen(pFilePath, pOpenMode);
#else
    #if defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64 && defined(_LARGEFILE64_SOURCE)
        *ppFile = fopen64(pFilePath, pOpenMode);
    #else
        *ppFile = fopen(pFilePath, pOpenMode);
    #endif
#endif
    if (*ppFile == NULL) {
        mfs_result result = mfs_result_from_errno(errno);
        if (result == MFS_SUCCESS) {
            result = MFS_ERROR; /* Just a safety check to make sure we never ever return success when pFile == NULL. */
        }

        return result;
    }
#endif

    return MFS_SUCCESS;
}

/*
_wfopen() isn't always available in all compilation environments.

    * Windows only.
    * MSVC seems to support it universally as far back as VC6 from what I can tell (haven't checked further back).
    * MinGW-64 (both 32- and 64-bit) seems to support it.
    * MinGW wraps it in !defined(__STRICT_ANSI__).

This can be reviewed as compatibility issues arise. The preference is to use _wfopen_s() and _wfopen() as opposed to the wcsrtombs()
fallback, so if you notice your compiler not detecting this properly I'm happy to look at adding support.
*/
#if defined(_WIN32)
    #if defined(_MSC_VER) || defined(__MINGW64__) || !defined(__STRICT_ANSI__)
        #define MFS_HAS_WFOPEN
    #endif
#endif

mfs_result mfs_wfopen(FILE** ppFile, const wchar_t* pFilePath, const wchar_t* pOpenMode)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    errno_t err;
#endif

    if (ppFile != NULL) {
        *ppFile = NULL;  /* Safety. */
    }

    if (pFilePath == NULL || pOpenMode == NULL || ppFile == NULL) {
        return MFS_INVALID_ARGS;
    }

#if defined(MFS_HAS_WFOPEN)
    /* Use _wfopen() on Windows. */
    #if defined(_MSC_VER) && _MSC_VER >= 1400
        err = _wfopen_s(ppFile, pFilePath, pOpenMode);
        if (err != 0) {
            return mfs_result_from_errno(err);
        }
    #else
        *ppFile = _wfopen(pFilePath, pOpenMode);
        if (*ppFile == NULL) {
            return mfs_result_from_errno(errno);
        }
    #endif
#else
    /*
    Use fopen() on anything other than Windows. Requires a conversion. This is annoying because fopen() is locale specific. The only real way I can
    think of to do this is with wcsrtombs(). Note that wcstombs() is apparently not thread-safe because it uses a static global mbstate_t object for
    maintaining state. I've checked this with -std=c89 and it works, but if somebody get's a compiler error I'll look into improving compatibility.
    */
    {
        mbstate_t mbs;
        size_t lenMB;
        const wchar_t* pFilePathTemp = pFilePath;
        char* pFilePathMB = NULL;
        char pOpenModeMB[32] = {0};

        /* Get the length first. */
        MFS_ZERO_OBJECT(&mbs);
        lenMB = wcsrtombs(NULL, &pFilePathTemp, 0, &mbs);
        if (lenMB == (size_t)-1) {
            return mfs_result_from_errno(errno);
        }

        pFilePathMB = (char*)MFS_MALLOC(lenMB + 1);
        if (pFilePathMB == NULL) {
            return MFS_OUT_OF_MEMORY;
        }

        pFilePathTemp = pFilePath;
        MFS_ZERO_OBJECT(&mbs);
        wcsrtombs(pFilePathMB, &pFilePathTemp, lenMB + 1, &mbs);

        /* The open mode should always consist of ASCII characters so we should be able to do a trivial conversion. */
        {
            size_t i = 0;
            for (;;) {
                if (pOpenMode[i] == 0) {
                    pOpenModeMB[i] = '\0';
                    break;
                }

                pOpenModeMB[i] = (char)pOpenMode[i];
                i += 1;
            }
        }

        *ppFile = fopen(pFilePathMB, pOpenModeMB);

        MFS_FREE(pFilePathMB);
    }

    if (*ppFile == NULL) {
        return MFS_ERROR;
    }
#endif

    return MFS_SUCCESS;
}

mfs_result mfs_fclose(FILE* pFile)
{
    int result = fclose(pFile);
    if (result != 0) {
        return MFS_ERROR;
    }

    return MFS_SUCCESS;
}

mfs_result mfs_fread(FILE* pFile, void* pData, size_t sizeInBytes, size_t* pBytesRead)
{
    size_t result;

    result = fread(pData, 1, sizeInBytes, pFile);
    
    if (pBytesRead != NULL) {
        *pBytesRead = result;
    }
    
    if (result != sizeInBytes) {
        if (feof(pFile)) {
            return MFS_END_OF_FILE;
        } else {
            return mfs_result_from_errno(ferror(pFile));
        }
    }

    return MFS_SUCCESS;
}

mfs_result mfs_fwrite(FILE* pFile, const void* pData, size_t sizeInBytes, size_t* pBytesWritten)
{
    size_t result;

    result = fwrite(pData, 1, sizeInBytes, pFile);

    if (pBytesWritten != NULL) {
        *pBytesWritten = result;
    }

    if (result != sizeInBytes) {
        return mfs_result_from_errno(ferror(pFile));
    }

    return MFS_SUCCESS;
}

mfs_result mfs_fseek(FILE* pFile, mfs_int64 offset, int origin)
{
    int result;
#if defined(_WIN32)
    result = _fseeki64(pFile, offset, origin);
#else
    result = fseek(pFile, (long int)offset, origin);
#endif
    if (result != 0) {
        return MFS_ERROR;
    }

    return MFS_SUCCESS;
}

mfs_int64 mfs_ftell(FILE* pFile)
{
    mfs_int64 result;
#if defined(_WIN32)
    result = _ftelli64(pFile);
#else
    result = ftell(pFile);
#endif

    return result;
}

#if !defined(_MSC_VER) && !((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 1) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)) && !(defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__))
int fileno(FILE *stream);
#endif

mfs_result mfs_fstat(FILE* pFile, mfs_stat_info* info)
{
    int fd;

#if defined(_MSC_VER)
    fd = _fileno(pFile);
#else
    fd =  fileno(pFile);
#endif

#if defined(_MSC_VER)
    if (_fstat64(fd, info) != 0) {
#elif defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64 && defined(_LARGEFILE64_SOURCE)
    if (fstat64(fd, info) != 0) {
#else
    if (fstat(fd, info) != 0) {
#endif
        return mfs_result_from_errno(errno);
    }

    return MFS_SUCCESS;
}


static mfs_result mfs_open_and_read_file_with_extra_data(const char* pFilePath, size_t* pFileSizeOut, void** ppFileData, size_t extraBytes)
{
    mfs_result result;
    mfs_uint64 fileSize;
    FILE* pFile;
    void* pFileData;
    size_t bytesRead;

    if (pFilePath == NULL) {
        return MFS_INVALID_ARGS;
    }

    result = mfs_fopen(&pFile, pFilePath, "rb");
    if (result != MFS_SUCCESS) {
        return result;
    }

    fseek(pFile, 0, SEEK_END);
    fileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    if (fileSize+extraBytes > SIZE_MAX) {
        mfs_fclose(pFile);
        return MFS_TOO_BIG;
    }

    pFileData = MFS_MALLOC((size_t)fileSize + extraBytes);    /* <-- Safe cast due to the check above. */
    if (pFileData == NULL) {
        mfs_fclose(pFile);
        return MFS_OUT_OF_MEMORY;
    }

    result = mfs_fread(pFile, pFileData, (size_t)fileSize, &bytesRead);
    if (result != MFS_SUCCESS) {
        MFS_FREE(pFileData);
        fclose(pFile);
        return result;
    }

    fclose(pFile);

    if (pFileSizeOut) {
        *pFileSizeOut = (size_t)fileSize;
    }

    if (ppFileData) {
        *ppFileData = pFileData;
    } else {
        MFS_FREE(pFileData);
    }

    return MFS_SUCCESS;
}

mfs_result mfs_open_and_read_file(const char* pFilePath, size_t* pFileSizeOut, void** ppFileData)
{
    return mfs_open_and_read_file_with_extra_data(pFilePath, pFileSizeOut, ppFileData, 0);
}

mfs_result mfs_open_and_read_text_file(const char* pFilePath, size_t* pFileSizeOut, char** ppFileData)
{
    size_t fileSize;
    mfs_result result = mfs_open_and_read_file_with_extra_data(pFilePath, &fileSize, (void**)ppFileData, 1);    /* 1 extra byte for the null terminator. */
    if (result != MFS_SUCCESS) {
        return result;
    }

    /* Output data needs to be null terminated. */
    (*ppFileData)[fileSize] = '\0';

    if (pFileSizeOut != NULL) {
        *pFileSizeOut = fileSize;
    }

    return result;
}


mfs_result mfs_open_and_write_file(const char* pFilePath, size_t fileSize, const void* pFileData)
{
    mfs_result result;
    FILE* pFile;

    if (pFilePath == NULL) {
        return MFS_INVALID_ARGS;
    }

    result = mfs_fopen(&pFile, pFilePath, "wb");
    if (result != MFS_SUCCESS) {
        return result;
    }

    result = mfs_fwrite(pFile, pFileData, fileSize, NULL);
    mfs_fclose(pFile);

    return result;
}


/* Current Directory */

#if defined(MFS_WIN32)
mfs_result mfs_set_current_directory__win32(const char* pDirectoryPath)
{
    if (SetCurrentDirectoryA(pDirectoryPath)) {
        return MFS_SUCCESS;
    }

    return mfs_result_from_GetLastError(GetLastError());
}

mfs_result mfs_get_current_directory__win32(char* pDirectoryPath, size_t directoryPathSizeInBytes)
{
    DWORD result;

    result = GetCurrentDirectoryA((DWORD)directoryPathSizeInBytes, pDirectoryPath);
    if (result == 0) {
        return mfs_result_from_GetLastError(GetLastError());
    }

    /*
    It's not entirely clear by the documentation for GetCurrentDirectory() what the contents of the output buffer is set to in the scenario where
    it's not large enough to hold the result. For consistency with the POSIX implementation I'm going to explicitly set this to a blank string and
    return an error if the return value is larger than the size of the buffer.
    */
    if (result > (DWORD)directoryPathSizeInBytes) {
        pDirectoryPath[0] = '\0';
        return MFS_OUT_OF_RANGE;
    }

    return MFS_SUCCESS;
}

mfs_result mfs_mkdir__win32(const char* pDirectoryPath)
{
    BOOL result = CreateDirectoryA(pDirectoryPath, NULL);
    if (result) {
        return MFS_SUCCESS;
    }

    return mfs_result_from_GetLastError(GetLastError());
}
#endif

#if defined(MFS_POSIX)
mfs_result mfs_set_current_directory__posix(const char* pDirectoryPath)
{
    int result = chdir(pDirectoryPath);
    if (result != 0) {
        return mfs_result_from_errno(errno);
    }

    return MFS_SUCCESS;
}

mfs_result mfs_get_current_directory__posix(char* pDirectoryPath, size_t directoryPathSizeInBytes)
{
    int result;
    char* cwd;
    
    cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        return mfs_result_from_errno(errno);
    }

    result = mfs_strcpy_s(pDirectoryPath, directoryPathSizeInBytes, cwd);
    free(cwd);

    if (result != 0) {
        return mfs_result_from_errno(errno);
    }

    return MFS_SUCCESS;
}

mfs_result mfs_mkdir__posix(const char* pDirectoryPath)
{
    if (mkdir(pDirectoryPath, 0777) == 0) {
        return MFS_SUCCESS;
    }

    return mfs_result_from_errno(errno);
}
#endif

mfs_result mfs_set_current_directory(const char* pDirectoryPath)
{
#if defined(MFS_WIN32)
    return mfs_set_current_directory__win32(pDirectoryPath);
#elif defined(MFS_POSIX)
    return mfs_set_current_directory__posix(pDirectoryPath);
#else
    return MFS_INVALID_OPERATION;   /* Unsupported platform. */
#endif
}

mfs_result mfs_get_current_directory(char* pDirectoryPath, size_t directoryPathSizeInBytes)
{
    if (pDirectoryPath == NULL) {
        return MFS_INVALID_ARGS;
    }
    if (directoryPathSizeInBytes == 0) {
        return MFS_OUT_OF_RANGE;
    }

    pDirectoryPath[0] = '\0';

#if defined(MFS_WIN32)
    return mfs_get_current_directory__win32(pDirectoryPath, directoryPathSizeInBytes);
#elif defined(MFS_POSIX)
    return mfs_get_current_directory__posix(pDirectoryPath, directoryPathSizeInBytes);
#else
    return MFS_INVALID_OPERATION;   /* Unsupported platform. */
#endif
}

mfs_result mfs_mkdir(const char* pDirectory, mfs_bool32 recursive)
{
    if (pDirectory == NULL) {
        return MFS_INVALID_ARGS;
    }

    if (pDirectory[0] == '\0') {
        return MFS_SUCCESS; /* It's an empty directory which we'll treat as the current directory. The current directory must exist so we just pretend to be successful. */
    }

    if (recursive == MFS_FALSE) {
    #if defined(MFS_WIN32)
        return mfs_mkdir__win32(pDirectory);
    #elif defined(MFS_POSIX)
        return mfs_mkdir__posix(pDirectory);
    #else
        return MFS_INVALID_OPERATION;   /* Unsupported platform. */
    #endif
    } else {
        /*
        Getting here means we're creating the directory recursively. We keep iterating over each segment of the path until we find a directory that does
        not exist, after which point we just keep creating directories until we reach the end.
        */
        mfs_result result;
        mfs_path_iterator iterator;
        char* pRunningPath = NULL;
        size_t runningPathCap = 0;
        size_t newRunningPathLen;

        result = mfs_path_first_segment(pDirectory, &iterator);
        if (result != MFS_SUCCESS) {
            return result;
        }

        runningPathCap = 256;
        pRunningPath = (char*)MFS_MALLOC(runningPathCap);
        if (pRunningPath == NULL) {
            return MFS_OUT_OF_MEMORY;
        }
        mfs_strncpy_s(pRunningPath, runningPathCap, iterator.path + iterator.segment.offset, iterator.segment.length);


        /* If it's an absolute path we want to skip the first segment as that will be the root directory . */
        if (mfs_path_is_absolute(pDirectory)) {
            result = mfs_path_next_segment(&iterator);
            if (result != MFS_SUCCESS) {
                if (result == MFS_AT_END) {
                    return MFS_SUCCESS;
                } else {
                    return result;
                }
            }

            /* Get the size of the new running path. */
            result = mfs_path_append_iterator(NULL, 0, pRunningPath, iterator, &newRunningPathLen);
            if (result != MFS_SUCCESS) {
                MFS_FREE(pRunningPath);
                return result;
            }

            /* Allocate memory for the running path if necessary. */
            if (newRunningPathLen > runningPathCap+1) {
                char* pNewRunningPath;
                size_t newRunningPathCap = runningPathCap*2;
                if (newRunningPathCap < newRunningPathLen+1) {
                    newRunningPathCap = newRunningPathLen+1;
                }

                pNewRunningPath = (char*)MFS_REALLOC(pRunningPath, newRunningPathCap);
                if (pNewRunningPath == NULL) {
                    MFS_FREE(pRunningPath);
                    return MFS_OUT_OF_MEMORY;
                }

                pRunningPath   = pNewRunningPath;
                runningPathCap = newRunningPathCap;
            }

            /* Append the segment to the running path. */
            result = mfs_path_append_iterator(pRunningPath, runningPathCap, pRunningPath, iterator, NULL);
            if (result != MFS_SUCCESS) {
                MFS_FREE(pRunningPath);
                return result;  /* Should never hit this as any error should have been returned by the first call that we used to measure the string. */
            }
        }

        /*
        We need to loop over every segment until we find the first directory that does not exist. After that we then just create each sub-directory
        until we reach the end of the path.
        */
        for (;;) {
            /* Now that we have the running path we can check whether or not it exists. If it does not exists, it's created, so long as we're not looking at a file in which case we have an error. */
            if (mfs_file_exists(pRunningPath)) {
                MFS_FREE(pRunningPath);
                return MFS_INVALID_OPERATION;   /* The path refers to a file. */
            }

            if (mfs_is_directory(pRunningPath) == MFS_FALSE) {
            #if defined(MFS_WIN32)
                result = mfs_mkdir__win32(pRunningPath);
            #elif defined(MFS_POSIX)
                result = mfs_mkdir__posix(pRunningPath);
            #else
                result = MFS_INVALID_OPERATION;   /* Unsupported platform. */
            #endif
                if (result != MFS_SUCCESS) {
                    MFS_FREE(pRunningPath);
                    return result;  /* An error occurred when creating the directory. */
                }
            }

            /* We're done with this segment so move on to the next. */
            result = mfs_path_next_segment(&iterator);
            if (result != MFS_SUCCESS) {
                if (result == MFS_AT_END) {
                    break;  /* We're done. */
                } else {
                    MFS_FREE(pRunningPath);
                    return result;
                }
            }

            /* Get the size of the new running path. */
            result = mfs_path_append_iterator(NULL, 0, pRunningPath, iterator, &newRunningPathLen);
            if (result != MFS_SUCCESS) {
                MFS_FREE(pRunningPath);
                return result;
            }

            /* Allocate memory for the running path if necessary. */
            if (newRunningPathLen > runningPathCap+1) {
                char* pNewRunningPath;
                size_t newRunningPathCap = runningPathCap*2;
                if (newRunningPathCap < newRunningPathLen+1) {
                    newRunningPathCap = newRunningPathLen+1;
                }

                pNewRunningPath = (char*)MFS_REALLOC(pRunningPath, newRunningPathCap);
                if (pNewRunningPath == NULL) {
                    MFS_FREE(pRunningPath);
                    return MFS_OUT_OF_MEMORY;
                }

                pRunningPath   = pNewRunningPath;
                runningPathCap = newRunningPathCap;
            }

            /* Append the segment to the running path. */
            result = mfs_path_append_iterator(pRunningPath, runningPathCap, pRunningPath, iterator, NULL);
            if (result != MFS_SUCCESS) {
                MFS_FREE(pRunningPath);
                return result;  /* Should never hit this as any error should have been returned by the first call that we used to measure the string. */
            }
        }

        MFS_FREE(pRunningPath);
        return MFS_SUCCESS;
    }
}

mfs_result mfs_rmdir(const char* pDirectory, mfs_bool32 recursive)
{
    if (pDirectory == NULL) {
        return MFS_INVALID_ARGS;
    }

    if (mfs_is_directory(pDirectory) == MFS_FALSE) {
        return MFS_NOT_DIRECTORY;
    }

    if (recursive) {
        mfs_result result = mfs_rmdir_content(pDirectory);
        if (result != MFS_SUCCESS) {
            return result;  /* Failed to delete the content of the directory. */
        }
    }

    return mfs_delete_file(pDirectory);
}

mfs_result mfs_rmdir_content(const char* pDirectory)
{
    /* We'll use an iterator for this. */
    mfs_result result;
    mfs_iterator iterator;
    mfs_file_info fi;

    if (pDirectory == NULL) {
        return MFS_INVALID_ARGS;
    }

    result = mfs_iterator_init(pDirectory, &iterator);
    if (result != MFS_SUCCESS) {
        return result;  /* Failed to initialize iterator. */
    }

    while (mfs_iterator_next(&iterator, &fi) == MFS_SUCCESS) {
        char* pFilePath;
        size_t filePathLen;

        /* Get the length first. */
        result = mfs_path_append(NULL, 0, pDirectory, fi.pFileName, &filePathLen);
        if (result == MFS_SUCCESS) {
            pFilePath = (char*)MFS_MALLOC(filePathLen + 1);    /* +1 for null terminator. */
            if (pFilePath != NULL) {
                result = mfs_path_append(pFilePath, filePathLen + 1, pDirectory, fi.pFileName, NULL);
                if (result == MFS_SUCCESS) {
                    if (fi.isDirectory) {
                        if (fi.pFileName[0] == '.' && fi.pFileName[1] == '\0') {
                            /* "." - ignore. */
                        } else if (fi.pFileName[0] == '.' && fi.pFileName[1] == '.' && fi.pFileName[2] == '\0') {
                            /* ".." - ignore. */
                        } else {
                            mfs_rmdir(pFilePath, MFS_TRUE);
                        }
                    } else {
                        mfs_delete_file(pFilePath);
                    }
                } else {
                    /* Failed to create file path. */
                }
            } else {
                return MFS_OUT_OF_MEMORY;
            }
        } else {
            /* Failed to retrieve the file path length. */
        }
    }

    return MFS_SUCCESS;
}


#if defined(MFS_WIN32)
mfs_bool32 mfs_is_directory__win32(const char* pPath)
{
    DWORD attributes;

    MFS_ASSERT(pPath != NULL);

    attributes = GetFileAttributesA(pPath);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

mfs_bool32 mfs_file_exists__win32(const char* pFilePath)
{
    DWORD attributes;

    MFS_ASSERT(pFilePath != NULL);

    attributes = GetFileAttributesA(pFilePath);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

mfs_result mfs_copy_file__win32(const char* pSrcFilePath, const char* pDstFilePath, mfs_bool32 failIfExists)
{
    BOOL result = CopyFileA(pSrcFilePath, pDstFilePath, failIfExists);
    if (result) {
        return MFS_SUCCESS;
    }

    return mfs_result_from_GetLastError(GetLastError());
}

mfs_result mfs_move_file__win32(const char* pSrcFilePath, const char* pDstFilePath, mfs_bool32 failIfExists)
{
    DWORD dwFlags;
    BOOL result;

    dwFlags = MOVEFILE_COPY_ALLOWED;
    if (failIfExists == MFS_FALSE) {
        dwFlags |= MOVEFILE_REPLACE_EXISTING;
    }

    result = MoveFileExA(pSrcFilePath, pDstFilePath, dwFlags);
    if (result) {
        return MFS_SUCCESS;
    }

    return mfs_result_from_GetLastError(GetLastError());
}

mfs_result mfs_delete_file__win32(const char* pFilePath)
{
    BOOL wasSuccessful;
    DWORD attributes;

    attributes = GetFileAttributesA(pFilePath);
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
        wasSuccessful = DeleteFileA(pFilePath);
    } else {
        wasSuccessful = RemoveDirectoryA(pFilePath);
    }

    if (wasSuccessful) {
        return MFS_SUCCESS;
    }

    return mfs_result_from_GetLastError(GetLastError());
}
#endif

#if defined(MFS_POSIX)
mfs_bool32 mfs_is_directory__posix(const char* pPath)
{
    struct stat info;

    MFS_ASSERT(pPath != NULL);

    if (stat(pPath, &info) != 0) {
        return MFS_FALSE;   /* Likely the folder doesn't exist. */
    }

    return (info.st_mode & S_IFDIR) != 0;
}

mfs_bool32 mfs_file_exists__posix(const char* pFilePath)
{
    struct stat info;

    MFS_ASSERT(pFilePath != NULL);
    
    if (stat(pFilePath, &info) != 0) {
        return MFS_FALSE;   /* Likely the file doesn't exist. */
    }

    return (info.st_mode & S_IFDIR) == 0;
}

mfs_result mfs_copy_file__posix(const char* pSrcFilePath, const char* pDstFilePath, mfs_bool32 failIfExists)
{
    mfs_result res;
    int inFd, outFd;
    struct stat info;
    ssize_t writeBytes, readBytes, writtenBytes;
    char buff[4096];

    /* Checks weather the destination file exists. */
    if (failIfExists && stat(pDstFilePath, &info) == 0) {
        return MFS_ALREADY_EXISTS;
    }

    /* Acquire a file descriptor for the input file. */
    inFd = open(pSrcFilePath, O_RDONLY);
    if (inFd < 0) {
        res = mfs_result_from_errno(errno);
        return res;
    }

    /* Retrieve mode information of the input file. */
    if (fstat(inFd, &info) != 0) {
        res = mfs_result_from_errno(errno);
        close(inFd);
        return res;
    }

    /* Create an output file and acquire its FD. */
    outFd = open(pDstFilePath, O_CREAT | O_WRONLY | O_TRUNC, info.st_mode);
    if (outFd < 0) {
        res = mfs_result_from_errno(errno);
        close(inFd);
        return res;
    }

    /* Perform file copy in chunks until end of file. */
    do {
        readBytes = read(inFd, buff, sizeof(buff));

        if (readBytes < 0) { /* Read error. */
            res = mfs_result_from_errno(errno);
            close(inFd);
            close(outFd);
            return res;
        } else if (readBytes > 0) { /* Read some bytes. */
            writtenBytes = 0;
            do { /* Keep writing until all read bytes are written. */
                writeBytes = write(outFd, &buff[writtenBytes], readBytes - writtenBytes);
                if (writeBytes < 0) { /* Write error. */
                    res = mfs_result_from_errno(errno);
                    close(inFd);
                    close(outFd);
                    return res;
                }
                /* The write may be incomplete, thus we should keep writing until finished or an error. */
                writtenBytes += writeBytes;
            } while (writtenBytes < readBytes);
        }
    } while (readBytes > 0);

    /* Close both FDs. */
    close(outFd);
    close(inFd);

    return MFS_SUCCESS;
}

mfs_result mfs_move_file__posix(const char* pSrcFilePath, const char* pDstFilePath, mfs_bool32 failIfExists)
{
    if (rename(pSrcFilePath, pDstFilePath) == 0) {
        return MFS_SUCCESS;
    }

    return mfs_result_from_errno(errno);
}

mfs_result mfs_delete_file__posix(const char* pFilePath)
{
    if (remove(pFilePath) == 0) {
        return MFS_SUCCESS;
    }

    return mfs_result_from_errno(errno);
}
#endif

mfs_bool32 mfs_is_directory(const char* pPath)
{
    if (pPath == NULL) {
        return MFS_FALSE;
    }

#if defined(MFS_WIN32)
    return mfs_is_directory__win32(pPath);
#elif defined(MFS_POSIX)
    return mfs_is_directory__posix(pPath);
#else
    return MFS_FALSE;
#endif
}

mfs_bool32 mfs_file_exists(const char* pFilePath)
{
    if (pFilePath == NULL) {
        return MFS_FALSE;
    }
    
#if defined(MFS_WIN32)
    return mfs_file_exists__win32(pFilePath);
#elif defined(MFS_POSIX)
    return mfs_file_exists__posix(pFilePath);
#else
    return MFS_FALSE;
#endif
}


mfs_result mfs_copy_file(const char* pSrcFilePath, const char* pDstFilePath, mfs_bool32 failIfExists)
{
    if (pSrcFilePath == NULL || pDstFilePath == NULL) {
        return MFS_INVALID_ARGS;
    }

#if defined(MFS_WIN32)
    return mfs_copy_file__win32(pSrcFilePath, pDstFilePath, failIfExists);
#elif defined(MFS_POSIX)
    return mfs_copy_file__posix(pSrcFilePath, pDstFilePath, failIfExists);
#else
    return MFS_NOT_IMPLEMENTED;
#endif
}

mfs_result mfs_move_file(const char* pSrcFilePath, const char* pDstFilePath, mfs_bool32 failIfExists)
{
    if (pSrcFilePath == NULL || pDstFilePath == NULL) {
        return MFS_INVALID_ARGS;
    }

#if defined(MFS_WIN32)
    return mfs_move_file__win32(pSrcFilePath, pDstFilePath, failIfExists);
#elif defined(MFS_POSIX)
    return mfs_move_file__posix(pSrcFilePath, pDstFilePath, failIfExists);
#else
    return MFS_NOT_IMPLEMENTED;
#endif
}

mfs_result mfs_delete_file(const char* pFilePath)
{
    if (pFilePath == NULL) {
        return MFS_INVALID_ARGS;
    }

#if defined(MFS_WIN32)
    return mfs_delete_file__win32(pFilePath);
#elif defined(MFS_POSIX)
    return mfs_delete_file__posix(pFilePath);
#else
    return MFS_NOT_IMPLEMENTED;
#endif
}


#if defined(MFS_WIN32)
mfs_result mfs_get_file_info__win32(const char* pFilePath, mfs_file_info* pFileInfo)
{
    MFS_ASSERT(pFilePath != NULL);

    /* pFileInfo can be null, in which case it's equivalent to just checking if the file exist. */
    if (pFileInfo == NULL) {
        if (GetFileAttributesA(pFilePath) != INVALID_FILE_ATTRIBUTES) {
            return MFS_SUCCESS;
        }

        return MFS_DOES_NOT_EXIST;
    } else {
        WIN32_FILE_ATTRIBUTE_DATA fad;
        if (GetFileAttributesExA(pFilePath, GetFileExInfoStandard, &fad)) {
            ULARGE_INTEGER li;

            li.LowPart  = fad.nFileSizeLow;
            li.HighPart = fad.nFileSizeHigh;
            pFileInfo->sizeInBytes = li.QuadPart;

            li.LowPart  = fad.ftLastWriteTime.dwLowDateTime;
            li.HighPart = fad.ftLastWriteTime.dwHighDateTime;
            pFileInfo->lastModifiedTime = li.QuadPart;

            li.LowPart  = fad.ftLastAccessTime.dwLowDateTime;
            li.HighPart = fad.ftLastAccessTime.dwHighDateTime;
            pFileInfo->lastAccessTime = li.QuadPart;

            pFileInfo->isDirectory = (fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            pFileInfo->isReadOnly  = (fad.dwFileAttributes & FILE_ATTRIBUTE_READONLY)  != 0;

            return MFS_SUCCESS;
        }

        return MFS_DOES_NOT_EXIST;
    }
}
#elif defined(MFS_POSIX)
mfs_result mfs_get_file_info__posix(const char* pFilePath, mfs_file_info* pFileInfo)
{
    struct stat info;

    MFS_ASSERT(pFilePath != NULL);

    if (stat(pFilePath, &info) != 0) {
        return mfs_result_from_errno(errno);
    }

    if (pFileInfo != NULL) {
        pFileInfo->sizeInBytes      = info.st_size;
        pFileInfo->lastModifiedTime = info.st_mtime;
        pFileInfo->lastAccessTime   = info.st_atime;
        pFileInfo->isDirectory      = S_ISDIR(info.st_mode);
        pFileInfo->isReadOnly       = (access(pFilePath, W_OK) != 0);
    }

    return MFS_SUCCESS;
}
#endif

mfs_result mfs_get_file_info(const char* pFilePath, mfs_file_info* pFileInfo)
{
    if (mfs_string_is_null_or_empty(pFilePath)) {
        return MFS_INVALID_ARGS;
    }

    /*
    The caller will always have access to the original file path so I think it's OK to just copy as much of the name into the pre-allocated space
    as possible and just truncate the rest. This will cover the majority of cases. Since the caller already has access to the original path, I
    think this might be more useful to just store the file name portion rather than the whole path.
    */
    if (pFileInfo != NULL) {
        mfs_strncpy_s(pFileInfo->pFileName, sizeof(pFileInfo->pFileName), mfs_path_file_name(pFilePath), (size_t)-1);
    }

#if defined(MFS_WIN32)
    return mfs_get_file_info__win32(pFilePath, pFileInfo);
#elif defined(MFS_POSIX)
    return mfs_get_file_info__posix(pFilePath, pFileInfo);
#else
    return MFS_NOT_IMPLEMENTED; /* Unsupported platform. */
#endif
}


#if defined(MFS_WIN32)
mfs_result mfs_WIN32_FIND_DATAA_to_file_info(const WIN32_FIND_DATAA* pWin32FindData, mfs_file_info* pFileInfo)
{
    ULARGE_INTEGER li;

    MFS_ASSERT(pWin32FindData != NULL);
    MFS_ASSERT(pFileInfo != NULL);

    /* Slashes need to be normalized to forward slashes. */
    mfs_path_to_forward_slashes(pFileInfo->pFileName, sizeof(pFileInfo->pFileName), pWin32FindData->cFileName, NULL);

    li.LowPart  = pWin32FindData->nFileSizeLow;
    li.HighPart = pWin32FindData->nFileSizeHigh;
    pFileInfo->sizeInBytes = li.QuadPart;

    li.LowPart  = pWin32FindData->ftLastWriteTime.dwLowDateTime;
    li.HighPart = pWin32FindData->ftLastWriteTime.dwHighDateTime;
    pFileInfo->lastModifiedTime = li.QuadPart;

    li.LowPart  = pWin32FindData->ftLastAccessTime.dwLowDateTime;
    li.HighPart = pWin32FindData->ftLastAccessTime.dwHighDateTime;
    pFileInfo->lastAccessTime = li.QuadPart;
    
    pFileInfo->isDirectory = (pWin32FindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    pFileInfo->isReadOnly  = (pWin32FindData->dwFileAttributes & FILE_ATTRIBUTE_READONLY)  != 0;

    return MFS_SUCCESS;
}

mfs_result mfs_iterator_init__win32(const char* pDirectoryPath, mfs_iterator* pIterator)
{
    mfs_result result;
    char query[MAX_PATH];
    size_t queryLen;
    WIN32_FIND_DATAA fd;

    MFS_ASSERT(pDirectoryPath != NULL);
    MFS_ASSERT(pIterator != NULL);

    /* We need to make sure slashes are normalized to back slashes for Win32. */
    result = mfs_path_to_back_slashes(query, sizeof(query), pDirectoryPath, &queryLen);
    if (result != MFS_SUCCESS) {
        return result;
    }

    if (queryLen >= MAX_PATH-3) {
        return MFS_PATH_TOO_LONG;
    }

    /* An empty path means the current directory. */
    if (query[0] == '\0') {
        query[0] = '.';
        query[1] = '\0';
        queryLen += 1;
    }

    /* We need to add a wildcard, making sure we don't duplicate any trailing slash. */
    if (query[queryLen-1] == '\\' || query[queryLen-1] == '/') {
        query[queryLen+0] = '*';
        query[queryLen+1] = '\0';
        queryLen += 1;
    } else {
        query[queryLen+0] = '\\';
        query[queryLen+1] = '*';
        query[queryLen+2] = '\0';
        queryLen += 2;
    }

    pIterator->win32.handle = (mfs_handle)FindFirstFileA(query, &fd);
    if (pIterator->win32.handle == INVALID_HANDLE_VALUE) {
        pIterator->win32.handle = NULL;
        return mfs_result_from_GetLastError(GetLastError());
    };

    result = mfs_WIN32_FIND_DATAA_to_file_info(&fd, &pIterator->win32.fi);
    if (result != MFS_SUCCESS) {
        FindClose((HANDLE)pIterator->win32.handle);
        pIterator->win32.handle = NULL;
        return result;
    }

    pIterator->win32.atEnd = MFS_FALSE;

    return MFS_SUCCESS;
}

void mfs_iterator_uninit__win32(mfs_iterator* pIterator)
{
    MFS_ASSERT(pIterator != NULL);

    FindClose((HANDLE)pIterator->win32.handle);
    pIterator->win32.handle = NULL;
}

mfs_result mfs_iterator_next__win32(mfs_iterator* pIterator, mfs_file_info* pFileInfo)
{
    WIN32_FIND_DATAA fd;

    MFS_ASSERT(pIterator != NULL);

    if (pIterator->win32.atEnd) {
        return MFS_AT_END;
    }

    MFS_ASSERT(pIterator->win32.atEnd == MFS_FALSE);

    /* The file info will have already been retrieved. The first file is retrieved in mfs_iterator_init__win32(). */
    if (pFileInfo != NULL) {
        MFS_COPY_MEMORY(pFileInfo, &pIterator->win32.fi, sizeof(*pFileInfo));
    }

    /* Now we need to get the next file and store the info. */
    if (FindNextFileA((HANDLE)pIterator->win32.handle, &fd)) {
        mfs_WIN32_FIND_DATAA_to_file_info(&fd, &pIterator->win32.fi);
    } else {
        pIterator->win32.atEnd = MFS_TRUE;  /* Reached the end. */
    }

    return MFS_SUCCESS;
}
#endif

#if defined(MFS_POSIX)
mfs_result mfs_iterator_init__posix(const char* pDirectoryPath, mfs_iterator* pIterator)
{
    DIR* dir;
    size_t directoryPathLen;

    MFS_ASSERT(pDirectoryPath != NULL);
    MFS_ASSERT(pIterator != NULL);

    /* In our implementation, an empty string is equivalent to ".". */
    if (pDirectoryPath[0] == '\0') {
        pDirectoryPath = ".";
    }

    dir = opendir(pDirectoryPath);
    if (dir == NULL) {
        return mfs_result_from_errno(errno);
    }

    /* We need to keep track of the path so we can avoid changing the working directory in mfs_iterator_next__posix() when stat-ing the file. */
    directoryPathLen = strlen(pDirectoryPath);
    pIterator->posix.pPath = (char*)MFS_MALLOC(directoryPathLen + 1);   /* +1 for null terminator. */
    if (pIterator->posix.pPath == NULL) {
        closedir(dir);
        return MFS_OUT_OF_MEMORY;
    }
    mfs_strcpy_s(pIterator->posix.pPath, directoryPathLen+1, pDirectoryPath);

    pIterator->posix.dir = dir;

    return MFS_SUCCESS;
}

void mfs_iterator_uninit__posix(mfs_iterator* pIterator)
{
    MFS_ASSERT(pIterator != NULL);

    closedir((DIR*)pIterator->posix.dir);
    pIterator->posix.dir = NULL;

    MFS_FREE(pIterator->posix.pPath);
    pIterator->posix.pPath = NULL;
}

mfs_result mfs_iterator_next__posix(mfs_iterator* pIterator, mfs_file_info* pFileInfo)
{
    mfs_result result;
    struct dirent* info;
    struct stat statInfo;
    int statResult;
    mfs_bool32 hasWritePermissions;
    size_t filePathLen;
    char* pFilePath;
    char* pFilePathHeap = NULL;
    char  pFilePathStack[1024];

    MFS_ASSERT(pIterator != NULL);
    MFS_ASSERT(pFileInfo != NULL);

    info = readdir((DIR*)pIterator->posix.dir);
    if (info == NULL) {
        return MFS_AT_END;
    }

    /*
    We don't want to change the working directory as this has thread-safety implications. We instead need to append the file name to the
    directory path of the iterator.
    */
    result = mfs_path_append(NULL, 0, pIterator->posix.pPath, info->d_name, &filePathLen);
    if (result != MFS_SUCCESS) {
        return result;
    }

    if (filePathLen < sizeof(pFilePathStack)) {
        pFilePath = pFilePathStack;
    } else {
        pFilePathHeap = MFS_MALLOC(filePathLen + 1);
        if (pFilePathHeap == NULL) {
            return MFS_OUT_OF_MEMORY;
        }

        pFilePath = pFilePathHeap;
    }

    result = mfs_path_append(pFilePath, filePathLen+1, pIterator->posix.pPath, info->d_name, NULL);
    if (result != MFS_SUCCESS) {
        MFS_FREE(pFilePathHeap);
        return result;
    }

    statResult = stat(pFilePath, &statInfo);
    hasWritePermissions = (access(pFilePath, W_OK) == 0);

    MFS_FREE(pFilePathHeap);
    pFilePathHeap = NULL;
    pFilePath = NULL;

    if (statResult != 0) {
        return mfs_result_from_errno(errno);
    }

    mfs_strcpy_s(pFileInfo->pFileName, sizeof(pFileInfo->pFileName), info->d_name);
    pFileInfo->sizeInBytes      = statInfo.st_size;
    pFileInfo->lastModifiedTime = statInfo.st_mtime;
    pFileInfo->lastAccessTime   = statInfo.st_atime;
    pFileInfo->isDirectory      = S_ISDIR(statInfo.st_mode);
    pFileInfo->isReadOnly       = !hasWritePermissions;

    return MFS_SUCCESS;
}
#endif

mfs_result mfs_iterator_init(const char* pDirectoryPath, mfs_iterator* pIterator)
{
    if (pDirectoryPath == NULL || pIterator == NULL) {
        return MFS_INVALID_ARGS;
    }

    MFS_ZERO_OBJECT(pIterator);

#if defined(MFS_WIN32)
    return mfs_iterator_init__win32(pDirectoryPath, pIterator);
#elif defined(MFS_POSIX)
    return mfs_iterator_init__posix(pDirectoryPath, pIterator);
#else
    return MFS_INVALID_OPERATION;   /* Unsupported platform. */
#endif
}

void mfs_iterator_uninit(mfs_iterator* pIterator)
{
    if (pIterator == NULL) {
        return;
    }

#if defined(MFS_WIN32)
    mfs_iterator_uninit__win32(pIterator);
#elif defined(MFS_POSIX)
    mfs_iterator_uninit__posix(pIterator);
#else
    /* Unsupported platform. */
#endif

    MFS_ZERO_OBJECT(pIterator);
}

mfs_result mfs_iterator_next(mfs_iterator* pIterator, mfs_file_info* pFileInfo)
{
    if (pIterator == NULL) {
        return MFS_INVALID_ARGS;
    }

    if (pFileInfo != NULL) {
        MFS_ZERO_OBJECT(pFileInfo);
    }

#if defined(MFS_WIN32)
    return mfs_iterator_next__win32(pIterator, pFileInfo);
#elif defined(MFS_POSIX)
    return mfs_iterator_next__posix(pIterator, pFileInfo);
#else
    return MFS_INVALID_OPERATION;
#endif
}



/* Paths */
mfs_bool32 mfs_path_segments_equal(const char* s0Path, const mfs_path_segment s0, const char* s1Path, const mfs_path_segment s1)
{
    if (s0Path == NULL || s1Path == NULL) {
        return MFS_FALSE;
    }

    if (s0.length != s1.length) {
        return MFS_FALSE;
    }

    return strncmp(s0Path + s0.offset, s1Path + s1.offset, s0.length) == 0;
}


mfs_result mfs_path_first_segment(const char* path, mfs_path_iterator* pIterator)
{
    if (pIterator == NULL) {
        return MFS_INVALID_ARGS;
    }

    pIterator->path = path;
    pIterator->segment.offset = 0;
    pIterator->segment.length = 0;

    if (mfs_string_is_null_or_empty(path)) {
        return MFS_INVALID_ARGS;
    }

    while (pIterator->path[pIterator->segment.length] != '\0' && (pIterator->path[pIterator->segment.length] != '/' && pIterator->path[pIterator->segment.length] != '\\')) {
        pIterator->segment.length += 1;
    }

    return MFS_SUCCESS;
}

mfs_result mfs_path_last_segment(const char* path, mfs_path_iterator* pIterator)
{
    if (pIterator == NULL) {
        return MFS_INVALID_ARGS;
    }

    pIterator->path = path;
    pIterator->segment.offset = 0;
    pIterator->segment.length = 0;

    if (mfs_string_is_null_or_empty(path)) {
        return MFS_INVALID_ARGS;
    }

    pIterator->path = path;
    pIterator->segment.offset = strlen(path);
    pIterator->segment.length = 0;

    return mfs_path_prev_segment(pIterator);
}

mfs_result mfs_path_next_segment(mfs_path_iterator* pIterator)
{
    if (pIterator == NULL || pIterator->path == NULL) {
        return MFS_INVALID_ARGS;
    }

    pIterator->segment.offset = pIterator->segment.offset + pIterator->segment.length;
    pIterator->segment.length = 0;

    for (;;) {
        char c = pIterator->path[pIterator->segment.offset];
        if (c == '\0' || (c != '/' && c != '\\')) {
            break;
        }

        pIterator->segment.offset += 1;
    }

    if (pIterator->path[pIterator->segment.offset] == '\0') {
        return MFS_AT_END;
    }

    for (;;) {
        char c = pIterator->path[pIterator->segment.offset + pIterator->segment.length];
        if (c == '\0' || c == '/' || c == '\\') {
            break;
        }

        pIterator->segment.length += 1;
    }

    return MFS_SUCCESS;
}

mfs_result mfs_path_prev_segment(mfs_path_iterator* pIterator)
{
    size_t offsetEnd;

    if (pIterator == NULL || pIterator->path == NULL || pIterator->segment.offset == 0) {
        return MFS_INVALID_ARGS;
    }

    pIterator->segment.length = 0;

    do
    {
        pIterator->segment.offset -= 1;
    } while (pIterator->segment.offset > 0 && (pIterator->path[pIterator->segment.offset] == '/' || pIterator->path[pIterator->segment.offset] == '\\'));

    if (pIterator->segment.offset == 0) {
        if (pIterator->path[pIterator->segment.offset] == '/' || pIterator->path[pIterator->segment.offset] == '\\') {
            pIterator->segment.length = 0;
            return MFS_SUCCESS;
        } else {
            return MFS_AT_END;
        }
    }

    offsetEnd = pIterator->segment.offset + 1;
    while (pIterator->segment.offset > 0 && (pIterator->path[pIterator->segment.offset] != '/' && pIterator->path[pIterator->segment.offset] != '\\')) {
        pIterator->segment.offset -= 1;
    }

    if (pIterator->path[pIterator->segment.offset] == '/' || pIterator->path[pIterator->segment.offset] == '\\') {
        pIterator->segment.offset += 1;
    }

    pIterator->segment.length = offsetEnd - pIterator->segment.offset;

    return MFS_SUCCESS;
}

mfs_bool32 mfs_path_at_start(mfs_path_iterator iterator)
{
    return iterator.path != 0 && iterator.segment.offset == 0;
}

mfs_bool32 mfs_path_at_end(mfs_path_iterator iterator)
{
    return iterator.path == 0 || iterator.path[iterator.segment.offset] == '\0';
}

mfs_bool32 mfs_path_iterators_equal(const mfs_path_iterator i0, const mfs_path_iterator i1)
{
    return mfs_path_segments_equal(i0.path, i0.segment, i1.path, i1.segment);
}


mfs_bool32 mfs_path_is_root(const char* path)
{
    return mfs_path_is_unix_style_root(path) || mfs_path_is_win32_style_root(path);
}

mfs_bool32 mfs_path_is_root_segment(const char* path, const mfs_path_segment segment)
{
    return mfs_path_is_unix_style_root_segment(path, segment) || mfs_path_is_win32_style_root_segment(path, segment);
}

mfs_bool32 mfs_path_is_unix_style_root(const char* path)
{
    if (path == NULL) {
        return MFS_FALSE;
    }

    if (path[0] == '/') {
        return MFS_TRUE;
    }

    return MFS_FALSE;
}

mfs_bool32 mfs_path_is_unix_style_root_segment(const char* path, const mfs_path_segment segment)
{
    if (path == NULL) {
        return MFS_FALSE;
    }

    if (segment.offset == 0 && segment.length == 0) {
        return MFS_TRUE;    /* "/" style root. */
    }

    return MFS_FALSE;
}

mfs_bool32 mfs_path_is_win32_style_root(const char* path)
{
    if (path == NULL) {
        return MFS_FALSE;
    }

    if (((path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' && path[0] <= 'Z')) && path[1] == ':' && path[2] == '\0') {
        return MFS_TRUE;
    }

    return MFS_FALSE;
}

mfs_bool32 mfs_path_is_win32_style_root_segment(const char* path, const mfs_path_segment segment)
{
    if (path == NULL) {
        return MFS_FALSE;
    }

    if (segment.offset == 0 && segment.length == 2) {
        if (((path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' && path[0] <= 'Z')) && path[1] == ':') {
            return MFS_TRUE;
        }
    }

    return MFS_FALSE;
}


mfs_result mfs_path_copy(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut)
{
    size_t i;

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    /* The destination can be null in which case we're just measuring the string, which in this case is just a string length. */
    if (dst == NULL) {
        if (pDstLenOut == NULL) {
            return MFS_INVALID_ARGS;    /* If the destination is NULL it means we're retrieving the length. If the length output variable is also NULL we must have an error. */
        }

        if (src == NULL) {
            return MFS_INVALID_ARGS;
        }

        *pDstLenOut = strlen(src);
        return MFS_SUCCESS;
    }

    if (dstSizeInBytes == 0) {
        return MFS_OUT_OF_RANGE;
    }

    if (src == NULL) {
        dst[0] = '\0';
        return MFS_INVALID_ARGS;
    }

    for (i = 0; i < dstSizeInBytes && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (i < dstSizeInBytes) {
        dst[i] = '\0';

        if (pDstLenOut != NULL) {
            *pDstLenOut = i;
        }

        return MFS_SUCCESS;
    }

    dst[0] = '\0';
    return MFS_OUT_OF_RANGE;
}

static mfs_result mfs_path_normalize_separator(char* dst, size_t dstSizeInBytes, const char* src, char oldSeparator, char newSeparator, size_t* pDstLenOut)
{
    size_t i;

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    /* The destination can be null in which case we're just measuring the string, which in this case is just a string length. */
    if (dst == NULL) {
        if (pDstLenOut == NULL) {
            return MFS_INVALID_ARGS;    /* If the destination is NULL it means we're retrieving the length. If the length output variable is also NULL we must have an error. */
        }

        if (src == NULL) {
            return MFS_INVALID_ARGS;
        }

        *pDstLenOut = strlen(src);
        return MFS_SUCCESS;
    }

    if (dstSizeInBytes == 0) {
        return MFS_OUT_OF_RANGE;
    }

    if (src == NULL) {
        dst[0] = '\0';
        return MFS_INVALID_ARGS;
    }

    for (i = 0; i < dstSizeInBytes && src[i] != '\0'; ++i) {
        if (src[i] == oldSeparator) {
            dst[i] =  newSeparator;
        } else {
            dst[i] = src[i];
        }
    }

    if (i < dstSizeInBytes) {
        dst[i] = '\0';

        if (pDstLenOut != NULL) {
            *pDstLenOut = i;
        }

        return MFS_SUCCESS;
    }

    dst[0] = '\0';
    return MFS_OUT_OF_RANGE;
}

mfs_result mfs_path_to_forward_slashes(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut)
{
    return mfs_path_normalize_separator(dst, dstSizeInBytes, src, '\\', '/', pDstLenOut);
}

mfs_result mfs_path_to_back_slashes(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut)
{
    return mfs_path_normalize_separator(dst, dstSizeInBytes, src, '/', '\\', pDstLenOut);
}


mfs_bool32 mfs_path_is_descendant(const char* descendantAbsolutePath, const char* parentAbsolutePath)
{
    mfs_path_iterator iChild;
    mfs_path_iterator iParent;

    if (mfs_path_first_segment(descendantAbsolutePath, &iChild) != MFS_SUCCESS) {
        return MFS_FALSE;   /* The descendant is an empty string which makes it impossible for it to be a descendant. */
    }

    if (mfs_path_first_segment(parentAbsolutePath, &iParent) == MFS_SUCCESS) {
        do {
            /* If the segment is different, the paths are different and thus it is not a descendant. */
            if (!mfs_path_iterators_equal(iParent, iChild)) {
                return MFS_FALSE;
            }

            if (mfs_path_next_segment(&iChild) != MFS_SUCCESS) {
                return MFS_FALSE;   /* The descendant is shorter which means it's impossible for it to be a descendant. */
            }
        } while (mfs_path_next_segment(&iParent) == MFS_SUCCESS);
    }

    return MFS_TRUE;
}

mfs_bool32 mfs_path_is_child(const char* childAbsolutePath, const char* parentAbsolutePath)
{
    mfs_path_iterator iChild;
    mfs_path_iterator iParent;

    if (mfs_path_first_segment(childAbsolutePath, &iChild) != MFS_SUCCESS) {
        return MFS_FALSE;   /* The descendant is an empty string which makes it impossible for it to be a descendant. */
    }

    if (mfs_path_first_segment(parentAbsolutePath, &iParent) == MFS_SUCCESS) {
        do {
            /* If the segment is different, the paths are different and thus it is not a descendant. */
            if (mfs_path_iterators_equal(iParent, iChild)) {
                return MFS_FALSE;
            }

            if (mfs_path_next_segment(&iChild) != MFS_SUCCESS) {
                return MFS_FALSE;   /* The descendant is shorter which means it's impossible for it to be a descendant. */
            }
        } while (mfs_path_next_segment(&iParent) == MFS_SUCCESS);
    }

    /*
    At this point we have finished iteration of the parent, which should be shorter one. We now do one more iteration of
    the child to ensure it is indeed a direct child and not a deeper descendant.
    */
    return mfs_path_next_segment(&iChild) != MFS_SUCCESS;
}


const char* mfs_path_file_name(const char* path)
{
    const char* pFileName;

    if (path == NULL) {
        return NULL;
    }

    pFileName = path;

    /* We just loop through the path until we find the last slash. */
    while (path[0] != '\0') {
        if (path[0] == '/' || path[0] == '\\') {
            pFileName = path;
        }

        path += 1;
    }

    /* At this point the file name is sitting on a slash, so just move forward. */
    while (pFileName[0] != '\0' && (pFileName[0] == '/' || pFileName[0] == '\\')) {
        pFileName += 1;
    }

    return pFileName;
}


const char* mfs_path_extension(const char* path)
{
    const char* pFileName;
    const char* pLastOccurance;
    
    pFileName = mfs_path_file_name(path);
    if (pFileName == NULL) {
        return NULL;
    }

    pLastOccurance = NULL;

    /* We just find the last occurance of a period. If no period is found we just return an empty string. */
    while (*pFileName != '\0') {
        if (*pFileName == '.') {
            pFileName += 1;
            pLastOccurance = pFileName;
        } else {
            pFileName += 1;
        }
    }

    /*
    If there's no extension we need to return an empty string, but we need the return value to be an offset of the input path
    which means we actually want to return pFileName (which will be sitting on the null terminator at this point). The reason
    we want to always want the return value to be an offset of path is to make it so the caller can always calculate the length
    using a subtraction rather than an strlen().
    */
    if (pLastOccurance == NULL) {
        MFS_ASSERT(pFileName[0] == '\0');
        return pFileName;
    } else {
        return pLastOccurance;
    }
}

mfs_bool32 mfs_path_has_extension(const char* path)
{
    const char* ext = mfs_path_extension(path);
    if (ext == NULL || ext[0] == '\0') {
        return MFS_FALSE;
    }

    return MFS_TRUE;
}


mfs_bool32 mfs_path_equal(const char* path1, const char* path2)
{
    mfs_path_iterator iPath1;
    mfs_path_iterator iPath2;

    if (path1 == NULL || path2 == NULL) {
        return MFS_FALSE;
    }

    if (path1 == path2 || (path1[0] == '\0' && path2[0] == '\0')) {
        return MFS_TRUE;    /* Two empty paths are treated as the same. */
    }

    
    if (mfs_path_first_segment(path1, &iPath1) == MFS_SUCCESS && mfs_path_first_segment(path2, &iPath2) == MFS_SUCCESS) {
        mfs_bool32 isPath1Valid;
        mfs_bool32 isPath2Valid;

        do {
            if (!mfs_path_iterators_equal(iPath1, iPath2)) {
                return MFS_FALSE;
            }

            isPath1Valid = mfs_path_next_segment(&iPath1) == MFS_SUCCESS;
            isPath2Valid = mfs_path_next_segment(&iPath2) == MFS_SUCCESS;
        } while (isPath1Valid && isPath2Valid);

        /* At this point either iPath1 and/or iPath2 have finished iterating. If both of them are at the end, the two paths are equal. */
        return isPath1Valid == isPath2Valid && iPath1.path[iPath1.segment.offset] == '\0' && iPath2.path[iPath2.segment.offset] == '\0';
    }

    return MFS_FALSE;
}

mfs_bool32 mfs_path_extension_equal(const char* path, const char* extension)
{
    const char* ext1;
    const char* ext2;

    if (path == NULL || extension == NULL) {
        return MFS_FALSE;
    }

    ext1 = extension;
    ext2 = mfs_path_extension(path);

#ifdef _MSC_VER
    return _stricmp(ext1, ext2) == 0;
#else
    return strcasecmp(ext1, ext2) == 0;
#endif
}


mfs_bool32 mfs_path_is_relative(const char* path)
{
    mfs_path_iterator iterator;

    if (path == NULL) {
        return MFS_FALSE;
    }

    if (mfs_path_first_segment(path, &iterator) == MFS_SUCCESS) {
        return !mfs_path_is_root_segment(iterator.path, iterator.segment);
    }

    /* We'll get here if the path is empty. We consider this to be a relative path. */
    return MFS_TRUE;
}

mfs_bool32 mfs_path_is_absolute(const char* path)
{
    return !mfs_path_is_relative(path);
}


mfs_result mfs_path_base_path(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut)
{
    mfs_result result = MFS_SUCCESS;
    const char* dstorig;
    const char* baseend;
    size_t dstLen;

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    if (dst != NULL && dstSizeInBytes > 0) {
        dst[0] = '\0';
    }

    if (src == NULL) {
        return MFS_INVALID_ARGS;;
    }

    dstorig = src;
    baseend = src;

    /* We just loop through the path until we find the last slash. */
    while (src[0] != '\0') {
        if (src[0] == '/' || src[0] == '\\') {
            baseend = src;
        }

        src += 1;
    }

    /* Now we just loop backwards until we hit the first non-slash (this handles situations where there may be multiple path separators such as "C:/MyFolder////MyFile"). */
    while (baseend > src) {
        if (baseend[0] != '/' && baseend[0] != '\\') {
            break;
        }

        baseend -= 1;
    }

    dstLen = (baseend - dstorig);
    if (dst != NULL) {
        result = mfs_result_from_errno(mfs_strncpy_s(dst, dstSizeInBytes, dstorig, dstLen));
    }

    if (pDstLenOut != NULL) {
        *pDstLenOut = dstLen;
    }

    return result;
}

mfs_result mfs_path_file_name_without_extension(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut)
{
    return mfs_path_remove_extension(dst, dstSizeInBytes, mfs_path_file_name(src), pDstLenOut);
}


static mfs_result mfs_path_append__internal(char* dst, size_t dstSizeInBytes, const char* base, const char* other, size_t otherLen, size_t* pDstLenOut)
{
    mfs_result result = MFS_SUCCESS;
    size_t path1Length;
    size_t path2Length;
    size_t slashLength;
    size_t dstLen;

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    if (base == NULL) {
        base = "";
    }

    if (other == NULL) {
        other = "";
        otherLen = 0;
    }

    /*
    It only ever makes sense to "append" an absolute path to a blank path. If the other path is absolute, but the base
    path is not blank we need to return an error because it just doesn't make sense to do this.
    */
    if (mfs_path_is_absolute(other) && base[0] != '\0') {
        return MFS_INVALID_ARGS;
    }

    if (other[0] == '\0') {
        if (dst != NULL) {
            if (dst != base) {
                result = mfs_result_from_errno(mfs_strcpy_s(dst, dstSizeInBytes, base));
            }
        }

        if (pDstLenOut != NULL) {
            *pDstLenOut = strlen(base);
        }

        return result;
    }


    path1Length = strlen(base);
    path2Length = otherLen;
    slashLength = 0;

    if (path1Length > 0 && base[path1Length-1] != '/' && base[path1Length-1] != '\\') {
        slashLength = 1;
    }

    dstLen = path1Length + slashLength + path2Length;

    if (dst != NULL) {
        if (dstLen+1 <= dstSizeInBytes) {
            if (dst != base) {
                mfs_strncpy_s(dst, dstSizeInBytes, base, path1Length);
            }

            mfs_strncpy_s(dst + path1Length,               dstSizeInBytes - path1Length,               "/",   slashLength);
            mfs_strncpy_s(dst + path1Length + slashLength, dstSizeInBytes - path1Length - slashLength, other, path2Length);
        } else {
            result = MFS_OUT_OF_RANGE;
        }
    }

    if (pDstLenOut != NULL) {
        *pDstLenOut = dstLen;
    }

    return result;
}

mfs_result mfs_path_append(char* dst, size_t dstSizeInBytes, const char* base, const char* other, size_t* pDstLenOut)
{
    if (other == NULL) {
        return MFS_INVALID_ARGS;
    }

    return mfs_path_append__internal(dst, dstSizeInBytes, base, other, strlen(other), pDstLenOut);
}

mfs_result mfs_path_append_iterator(char* dst, size_t dstSizeInBytes, const char* base, mfs_path_iterator iterator, size_t* pDstLenOut)
{
    return mfs_path_append__internal(dst, dstSizeInBytes, base, iterator.path + iterator.segment.offset, iterator.segment.length, pDstLenOut);
}

mfs_result mfs_path_append_extension(char* dst, size_t dstSizeInBytes, const char* base, const char* extension, size_t* pDstLenOut)
{
    mfs_result result = MFS_SUCCESS;
    size_t baseLength;
    size_t extLength;
    size_t dstLen;

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    if (base == NULL) {
        base = "";
    }

    if (extension == NULL) {
        extension = "";
    }

    if (extension[0] == '\0') {
        if (dst != NULL) {
            if (dst != base) {
                result = mfs_result_from_errno(mfs_strcpy_s(dst, dstSizeInBytes, base));
            }
        }

        if (pDstLenOut != NULL) {
            *pDstLenOut = strlen(base);
        }
        
        return result;
    }


    baseLength = strlen(base);
    extLength  = strlen(extension);
    dstLen     = baseLength + 1 + extLength;

    if (dst != NULL) {
        if (dstLen+1 <= dstSizeInBytes) {
            if (dst != base) {
                mfs_strcpy_s(dst + 0, dstSizeInBytes - 0, base);
            }
            mfs_strcpy_s(dst + baseLength,     dstSizeInBytes - baseLength,     ".");
            mfs_strcpy_s(dst + baseLength + 1, dstSizeInBytes - baseLength - 1, extension);
        } else {
            result = MFS_OUT_OF_RANGE;
        }
    }

    if (pDstLenOut != NULL) {
        *pDstLenOut = dstLen;
    }

    return result;
}


static size_t mfs_path_clean__try_write(mfs_path_iterator* iterators, unsigned int iteratorCount, char* pathOut, size_t pathOutSize, unsigned int ignoreCounter)
{
    mfs_path_iterator iSegment;
    mfs_path_iterator iPrev;
    mfs_bool32 ignoreThisSegment;
    size_t bytesWritten;

    if (iteratorCount == 0) {
        return 0;
    }

    iSegment = iterators[iteratorCount - 1];


    /* If this segment is a ".", we ignore it. If it is a ".." we ignore it and increment "ignoreCount". */
    ignoreThisSegment = ignoreCounter > 0 && iSegment.segment.length > 0;

    if (iSegment.segment.length == 1 && iSegment.path[iSegment.segment.offset] == '.') {
        /* "." */
        ignoreThisSegment = MFS_TRUE;
    } else {
        if (iSegment.segment.length == 2 && iSegment.path[iSegment.segment.offset] == '.' && iSegment.path[iSegment.segment.offset + 1] == '.') {
            /* ".." */
            ignoreThisSegment = MFS_TRUE;
            ignoreCounter += 1;
        } else {
            /* It's a regular segment, so decrement the ignore counter. */
            if (ignoreCounter > 0) {
                ignoreCounter -= 1;
            }
        }
    }


    /* The previous segment needs to be written before we can write this one. */
    bytesWritten = 0;

    iPrev = iSegment;
    if (mfs_path_prev_segment(&iPrev) != MFS_SUCCESS) {
        if (iteratorCount > 1) {
            iteratorCount -= 1;
            iPrev = iterators[iteratorCount - 1];
        } else {
            iPrev.path           = NULL;
            iPrev.segment.offset = 0;
            iPrev.segment.length = 0;
        }
    }

    if (iPrev.segment.length > 0) {
        iterators[iteratorCount - 1] = iPrev;
        bytesWritten = mfs_path_clean__try_write(iterators, iteratorCount, pathOut, pathOutSize, ignoreCounter);
    }


    if (!ignoreThisSegment) {
        if (pathOut != NULL) {
            pathOut += bytesWritten;
            if (pathOutSize >= bytesWritten) {
                pathOutSize -= bytesWritten;
            } else {
                pathOutSize = 0;
            }
        }

        if (bytesWritten > 0) {
            if (pathOut != NULL) {
                pathOut[0] = '/';
                pathOut += 1;
                if (pathOutSize >= 1) {
                    pathOutSize -= 1;
                } else {
                    pathOutSize = 0;
                }
            }
            bytesWritten += 1;
        }

        if (pathOut != NULL) {
            MFS_COPY_MEMORY(pathOut, iSegment.path + iSegment.segment.offset, iSegment.segment.length); /* Don't use strncpy_s() here because we don't want to do anything with the null terminator at this point (that is set once at a higer level). */
        }

        bytesWritten += iSegment.segment.length;
    }

    return bytesWritten;
}

mfs_result mfs_path_clean(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut)
{
    mfs_result result = MFS_SUCCESS;
    mfs_path_iterator iLast;
    size_t bytesWritten = 0;

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    if (mfs_path_last_segment(src, &iLast) != MFS_SUCCESS) {
        return MFS_INVALID_ARGS;
    }

    
    if (src[0] == '/') {
        if (dst != NULL && dstSizeInBytes > 1) {
            dst[0] = '/';
        }
        bytesWritten = 1;
    }

    if (dst == NULL || dstSizeInBytes <= bytesWritten) {
        bytesWritten += mfs_path_clean__try_write(&iLast, 1, NULL, 0, 0);
    } else {
        bytesWritten += mfs_path_clean__try_write(&iLast, 1, dst + bytesWritten, dstSizeInBytes - bytesWritten - 1, 0);  /* -1 to ensure there is enough room for a null terminator later on. */
    }
        
    if (dst != NULL) {
        if (dstSizeInBytes > bytesWritten) {
            dst[bytesWritten] = '\0';
        } else {
            result = MFS_OUT_OF_RANGE;
        }
    }

    if (pDstLenOut != NULL) {
        *pDstLenOut = bytesWritten;
    }

    return result;
}

mfs_result mfs_path_append_and_clean(char* dst, size_t dstSizeInBytes, const char* base, const char* other, size_t* pDstLenOut)
{
    mfs_result result = MFS_SUCCESS;
    size_t bytesWritten;
    mfs_bool32 isPathEmpty0;
    mfs_bool32 isPathEmpty1;
    mfs_uint32 iteratorCount;
    mfs_path_iterator last[2] = {
        {NULL, {0, 0}},
        {NULL, {0, 0}}
    };

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    if (base == NULL || other == NULL) {
        return MFS_INVALID_ARGS;
    }

    isPathEmpty0 = mfs_path_last_segment(base,  last + 0) != MFS_SUCCESS;
    isPathEmpty1 = mfs_path_last_segment(other, last + 1) != MFS_SUCCESS;

    iteratorCount = !isPathEmpty0 + !isPathEmpty1;
    if (iteratorCount == 0) {
        return MFS_INVALID_ARGS;    /* Both input strings are empty. */
    }

    bytesWritten = 0;
    if (base[0] == '/') {
        if (dst != NULL && dstSizeInBytes > 1) {
            dst[0] = '/';
        }
        bytesWritten = 1;
    }

    if (dst == NULL || dstSizeInBytes <= bytesWritten) {
        bytesWritten += mfs_path_clean__try_write(last, 2, NULL, 0, 0);
    } else {
        bytesWritten += mfs_path_clean__try_write(last, 2, dst + bytesWritten, dstSizeInBytes - bytesWritten - 1, 0);  /* -1 to ensure there is enough room for a null terminator later on. */
    }

    if (dst != NULL && dstSizeInBytes > bytesWritten) {
        if (dstSizeInBytes > bytesWritten) {
            dst[bytesWritten] = '\0';
        } else {
            result = MFS_OUT_OF_RANGE;
        }
    }

    if (pDstLenOut != NULL) {
        *pDstLenOut = bytesWritten;
    }

    return result;
}


mfs_result mfs_path_remove_extension(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut)
{
    const char* ext;

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    if (src == NULL) {
        if (dst != NULL && dstSizeInBytes > 0) {
            dst[0] = '\0';
        }
        return MFS_INVALID_ARGS;
    }

    ext = mfs_path_extension(src);
    if (ext == NULL || ext[0] == '\0') {
        /* No extension. */
        if (dst == src) {
            *pDstLenOut = (ext - dst);
            return MFS_SUCCESS;
        } else {
            return mfs_path_copy(dst, dstSizeInBytes, src, pDstLenOut);
        }
    } else {
        /* Have extension. */
        size_t dstLen = (size_t)(ext - src - 1);    /* -1 for the period. */

        if (pDstLenOut != NULL) {
            *pDstLenOut = dstLen;
        }

        if (dst == src) {
            dst[dstLen] = '\0';
            return MFS_SUCCESS;
        } else {
            return mfs_result_from_errno(mfs_strncpy_s(dst, dstSizeInBytes, src, dstLen));
        }
    }
}

mfs_result mfs_path_remove_extension_in_place(char* path, size_t* pDstLenOut)
{
    const char* ext;

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    if (path == NULL) {
        return MFS_INVALID_ARGS;
    }

    ext = mfs_path_extension(path);
    if (ext == NULL || ext[0] == '\0') {
        /* No extension. */
        if (pDstLenOut == NULL) {
            *pDstLenOut = (ext - path);
        }
    } else {
        /* Have extension. */
        size_t dstLen = (size_t)(ext - path - 1);    /* -1 for the period. */

        if (pDstLenOut != NULL) {
            *pDstLenOut = dstLen;
        }

        path[dstLen] = '\0';
    }

    return MFS_SUCCESS;
}

mfs_result mfs_path_remove_file_name(char* dst, size_t dstSizeInBytes, const char* src, size_t* pDstLenOut)
{
    /*
    The tricky part with removing the file name is that we need to ensure we don't remove the root segment since it's not a file. To do this cleanly we're
    going to use a segment iterator find the file which will allow us to check for the root segment more easily.
    */
    mfs_result result = MFS_SUCCESS;
    mfs_path_iterator iLast;
    mfs_path_iterator iSecondLast;

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    if (src == NULL) {
        if (dst != NULL && dstSizeInBytes > 0) {
            dst[0] = '\0';
        }
        return MFS_INVALID_ARGS;
    }

    if (mfs_path_last_segment(src, &iLast) != MFS_SUCCESS) {
        return MFS_INVALID_ARGS;    /* The path is empty. */
    }

    /* Don't remove root segments. */
    if (mfs_path_is_root_segment(iLast.path, iLast.segment)) {
        return MFS_INVALID_ARGS;    /* The path is made up of only the root segment. */
    }

    /*
    If the last segment (the file name portion of the path) is the only segment, just return an empty string. Otherwise we copy
    up to the end of the second last segment.
    */
    iSecondLast = iLast;
    if (mfs_path_prev_segment(&iSecondLast) == MFS_SUCCESS) {
        size_t dstLen;
        if (mfs_path_is_unix_style_root_segment(iSecondLast.path, iSecondLast.segment)) {
            dstLen = iLast.segment.offset;
        } else {
            dstLen = iSecondLast.segment.offset + iSecondLast.segment.length;
        }

        if (dst != NULL) {
            result = mfs_result_from_errno(mfs_strncpy_s(dst, dstSizeInBytes, src, dstLen));
        }

        if (pDstLenOut != NULL) {
            *pDstLenOut = dstLen;
        }
    } else {
        if (dst != NULL) {
            if (dstSizeInBytes > 0) {
                dst[0] = '\0';
            } else {
                result = MFS_OUT_OF_RANGE;
            }
        }

        if (pDstLenOut != NULL) {
            *pDstLenOut = 0;
        }
    }

    return result;
}

mfs_result mfs_path_remove_file_name_in_place(char* path, size_t* pDstLenOut)
{
    mfs_path_iterator iLast;
    mfs_path_iterator iSecondLast;

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    if (path == NULL) {
        return MFS_INVALID_ARGS;
    }

    if (mfs_path_last_segment(path, &iLast) != MFS_SUCCESS) {
        return MFS_INVALID_ARGS;    /* The path is empty. */
    }

    /* Don't remove root segments. */
    if (mfs_path_is_root_segment(iLast.path, iLast.segment)) {
        return MFS_INVALID_ARGS;    /* The path is made up of only the root segment. */
    }

    /*
    If the last segment (the file name portion of the path) is the only segment, just return an empty string. Otherwise we copy
    up to the end of the second last segment.
    */
    iSecondLast = iLast;
    if (mfs_path_prev_segment(&iSecondLast) == MFS_SUCCESS) {
        size_t pathLen;
        if (mfs_path_is_unix_style_root_segment(iSecondLast.path, iSecondLast.segment)) {
            pathLen = iLast.segment.offset;
        } else {
            pathLen = iSecondLast.segment.offset + iSecondLast.segment.length;
        }

        path[pathLen] = '\0';

        if (pDstLenOut != NULL) {
            *pDstLenOut = pathLen;
        }
    } else {
        path[0] = '\0';

        if (pDstLenOut != NULL) {
            *pDstLenOut = 0;
        }
    }

    return MFS_SUCCESS;
}


mfs_result mfs_path_to_relative(char* dst, size_t dstSizeInBytes, const char* absolutePathToMakeRelative, const char* absolutePathToMakeRelativeTo, size_t* pDstLenOut)
{
    mfs_result result = MFS_SUCCESS;
    mfs_path_iterator iPath;
    mfs_path_iterator iBase;
    mfs_bool32 isPathEmpty;
    mfs_bool32 isBaseEmpty;
    int isPathAtEnd = 0;
    int isBaseAtEnd = 0;
    size_t dstLen;

    if (pDstLenOut != NULL) {
        *pDstLenOut = 0;
    }

    /*
    We do this in two phases. The first phase just iterates past each segment of both the path to convert and the
    base path until we find two that are not equal. The second phase just adds the appropriate ".." segments.
    */

    if (dst != NULL && dstSizeInBytes > 0) {
        dst[0] = '\0';
    }

    isPathEmpty = mfs_path_first_segment(absolutePathToMakeRelative,   &iPath) != MFS_SUCCESS;
    isBaseEmpty = mfs_path_first_segment(absolutePathToMakeRelativeTo, &iBase) != MFS_SUCCESS;

    if (isPathEmpty && isBaseEmpty) {
        return MFS_INVALID_ARGS;   /* Looks like both paths are empty. */
    }


    /* Phase 1: Get past the common section. */
    while (!isPathAtEnd && !isBaseAtEnd && mfs_path_iterators_equal(iPath, iBase)) {
        isPathAtEnd = mfs_path_next_segment(&iPath) != MFS_SUCCESS;
        isBaseAtEnd = mfs_path_next_segment(&iBase) != MFS_SUCCESS;
    }

    if (iPath.segment.offset == 0) {
        return MFS_INVALID_ARGS;   /* The path is not relative to the base path. */
    }


    /* Phase 2: Append ".." segments - one for each remaining segment in the base path. */
    dstLen = 0;

    if (!mfs_path_at_end(iBase)) {
        do {
            if (dstLen == 0) {
                /* It's the first segment, so we need to ensure we don't lead with a slash. */
                if (dst != NULL && dstLen+2 < dstSizeInBytes) {
                    dst[dstLen + 0] = '.';
                    dst[dstLen + 1] = '.';
                }
                dstLen += 2;
            } else {
                /* It's not the first segment. Make sure we lead with a slash. */
                if (dst != NULL && dstLen+3 < dstSizeInBytes) {
                    dst[dstLen + 0] = '/';
                    dst[dstLen + 1] = '.';
                    dst[dstLen + 2] = '.';
                }
                dstLen += 3;
            }
        } while (mfs_path_next_segment(&iBase) == MFS_SUCCESS);
    }


    /* Now we just append whatever is left of the main path. We want the path to be clean, so we append segment-by-segment. */
    if (!mfs_path_at_end(iPath)) {
        do {
            /* Leading slash, if required. */
            if (dstLen != 0) {
                if (dst != NULL && dstLen+1 < dstSizeInBytes) {
                    dst[dstLen] = '/';
                }
                dstLen += 1;
            }

            if (dst != NULL && dstLen+1 < dstSizeInBytes) {
                mfs_strncpy_s(dst + dstLen, dstSizeInBytes - dstLen, iPath.path + iPath.segment.offset, iPath.segment.length);
            }
            dstLen += iPath.segment.length;
        } while (mfs_path_next_segment(&iPath) == MFS_SUCCESS);
    }


    /* Always null terminate. */
    if (dst != NULL) {
        if (dstLen+1 <= dstSizeInBytes) {
            dst[dstLen] = '\0';
        } else {
            result = MFS_OUT_OF_RANGE;
        }
    }

    if (pDstLenOut != NULL) {
        *pDstLenOut = dstLen;
    }

    return result;
}

mfs_result mfs_path_to_absolute(char* dst, size_t dstSizeInBytes, const char* relativePathToMakeAbsolute, const char* basePath, size_t* pDstLenOut)
{
    return mfs_path_append_and_clean(dst, dstSizeInBytes, basePath, relativePathToMakeAbsolute, pDstLenOut);
}


void mfs_free(void* p)
{
    MFS_FREE(p);
}

#endif  /* minifs_c */
#endif  /* MINIFS_IMPLEMENTATION */


/*
This software is available as a choice of the following licenses. Choose
whichever you prefer.

===============================================================================
ALTERNATIVE 1 - Public Domain (www.unlicense.org)
===============================================================================
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

===============================================================================
ALTERNATIVE 2 - MIT No Attribution
===============================================================================
Copyright 2020 David Reid

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
