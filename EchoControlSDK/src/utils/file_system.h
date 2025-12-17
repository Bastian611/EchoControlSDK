
#pragma once
#include <vector>
#include <stdio.h>
#include "../global.h"

#ifdef __linux__
#include <limits.h>
#define MAX_PATH  PATH_MAX
#else
#include <Windows.h>
#endif

ECCS_BEGIN


bool isFile(const char* path);
bool isDirectory(const char* dir);
bool isFileExisted(const char* path);
bool isDirectoryExisted(const char* dir);
bool isFileDirectoryExisted(const char* path);

// returns 0 if success
int createDirectory(const char* dir);   // 递归创建
int deleteDirectory(const char* dir);
int copyFile(const char* src, const char* dst);
int changeCurrentDirectory(const char* dir);

str getDir(const char* filePath);
str getExecutablePath();
str getExecutableDirectory();
str getAboslutePath(const char* path);
str formatDirectory(str dir, bool slash); // slash: true - add slash at the end

//
// NOTE for Linux: std::wregex 需要设置合适的 C++ locale
// * std::locale::global(loc)
// * std::wregex::imbue(xxx)设置当前正则表达式需要的 locale
// 建议： 不要使用 std::locale::global(loc)，除非确定需要修改全局locale，
//       会影响stream相关的操作，比如整形数输出为 1,234,456
//

// returns < 0 if failed
i64 diskTotal(const char* path);
i64 diskFree(const char* path);
i64 fileSize(const char* path);
i64 directorySizeW(
        const char* dir,
        u32 maxdepth = 0,
        const wchar_t* regex = NULL);
i64 directorySize(
        const char* dir,
        u32 maxdepth = 0,
        const char* regex = NULL);

enum ScanMode{
    SCAN_FILE = 1,
    SCAN_DIR  = 2,
};
std::vector<str> scandirW(
    const char* dir,
    u32 mode,
    u32 maxdepth = 0,
    bool nameonly = true,
    const wchar_t* regex = NULL);
std::vector<str> scandir(
    const char* dir,
    u32 mode,
    u32 maxdepth = 0,
    bool nameonly = true,
    const char* regex = NULL);


ECCS_END
