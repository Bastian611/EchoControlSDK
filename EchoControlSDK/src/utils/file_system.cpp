
#include "file_system.h"
#include <fstream>
#include <memory>
#include <regex>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#endif
#include "../debug/str_error.h"
#include "../utils/utils.h"

ECCS_BEGIN


bool isFile(const char* path)
{
    return isFileExisted(path);
}
bool isDirectory(const char* dir)
{
    return isDirectoryExisted(dir);
}
bool isFileExisted(const char* path)
{
#ifdef _WIN32
    DWORD dwAttrib = GetFileAttributes(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    int iRet = stat(path, &st);
    return (iRet == -1) ? false : S_ISREG(st.st_mode);
#endif
}
bool isDirectoryExisted(const char* dir)
{
#ifdef _WIN32
    DWORD dwAttrib = GetFileAttributes(dir);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    int iRet = stat(dir, &st);
    return (iRet == -1) ? false : S_ISDIR(st.st_mode);
#endif
}
bool isFileDirectoryExisted(const char* path)
{
#ifdef _WIN32
    DWORD dwAttrib = GetFileAttributes(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES);
#else
    struct stat st;
    int iRet = stat(path, &st);
    return (iRet == -1) ? false : (S_ISDIR(st.st_mode) || S_ISREG(st.st_mode));
#endif
}

int createDirectory(const char* dir)
{
    size_t len = strlen(dir);
    char* path = new char[len+1];
    memset(path, 0, len+1);

    for (size_t i = 0; i < len; ++i)
    {
#ifdef _WIN32
        if (dir[i] != '\\' && dir[i] != '/' && i+1 < len) continue;
#else
        if (dir[i] != '/' && i+1 < len) continue;
#endif

        // copy dir when meet path delimiters or end of string
        memcpy(path, dir, i+1);

        // create if path is not existed
#ifdef _WIN32
        if (!isDirectoryExisted(path) && !CreateDirectory(path, NULL)){
            delete[] path;
            return -1;
        }
#else
        if (!isDirectoryExisted(path) && mkdir(path, 0775)){
            delete[] path;
            return -1;
        }
#endif
    }

    delete[] path;
    return 0;
}
int deleteDirectory(const char* dir)
{
    str path(dir);
    if (path.back() != '\\' && path.back() != '/'){
        path.append("/");
    }

#ifdef _WIN32
    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile((path+"*").c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return -1;
    }

    do
    {
        const char* fn = &ffd.cFileName[0];
        str newpath = path + fn;

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(fn, ".") && strcmp(fn, "..")) {
                deleteDirectory(newpath.c_str());
            }
        }
        else {
            DeleteFile(newpath.c_str());
        }
    } while (FindNextFile(hFind, &ffd));

    FindClose(hFind);

    if (RemoveDirectory(path.c_str()) != 0){
        return -1;
    }
#else
    DIR* dp = opendir(path.c_str());
    if (!dp){
        return -1;
    }

    dirent* dirp = NULL;
    while ((dirp = readdir(dp)) != NULL)
    {
        const char* fn = &dirp->d_name[0];
        str newpath = path + fn;
        if (dirp->d_type & DT_DIR) {
            if (strcmp(fn, ".") && strcmp(fn, "..")) {
                deleteDirectory(newpath.c_str());
            }
        }
        else if (dirp->d_type & DT_REG){
            remove(newpath.c_str());
        }
        else if(dirp->d_type == DT_UNKNOWN)
        {
            //针对非linux-ext2/3/4文件系统判断
            remove(newpath.c_str());
        }
    }

    closedir(dp);
    remove(path.c_str());
#endif

    return 0;
}
int copyFile(const char* src, const char* dst)
{
    std::ifstream ifs(src, std::ios::binary);
    if (!ifs.is_open()){
        return -1;
    }

    std::ofstream ofs(dst, std::ios::binary);
    if (!ofs.is_open()){
        return -2;
    }

    char buf[1024*4] = {0};
    while(ifs.good())
    {
        ifs.read(buf, sizeof(buf));
        auto cnt = ifs.gcount();
        if (cnt > 0){
            ofs.write(buf, cnt);
            if (!ofs.good()){
                return -3;
            }
        }
    }

    return 0;
}
int changeCurrentDirectory(const char* dir)
{
    return chdir(dir);
}

str getDir(const char* filePath)
{
    int idx = -1;

    const char* p = filePath;
    while (*p != 0)
    {
#ifdef _WIN32
        if (*p == '\\' || *p == '/'){
            idx = (int)(p-filePath);
        }
#else
        if (*p == '/'){
            idx = p-filePath;
        }
#endif
        ++p;
    }

    return str(filePath, idx+1);
}
str getExecutablePath()
{
#ifdef _WIN32
    char* path = NULL;
    if (_get_pgmptr(&path)) {
        path = NULL;
    }
#else
    char path[PATH_MAX] = {0};
    ssize_t len = ::readlink("/proc/self/exe", path, sizeof(path));
    if (len == -1 || len == sizeof(path)){
        len = 0;
    }
    path[len] = '\0';
#endif
    return str(path);
}
str getExecutableDirectory()
{
    str path = getExecutablePath();
#ifdef _WIN32
    return path.substr(0, path.find_last_of("\\/")+1);
#else
    return path.substr(0, path.find_last_of("/")+1);
#endif
}
str getAboslutePath(const char* path)
{
    if (!isFileDirectoryExisted(path)){
        return "";
    }

    char buf[MAX_PATH] = {0};
#ifdef _WIN32
    GetFullPathName(path, sizeof(buf), buf, NULL);
#else
    realpath(path, buf);
#endif
    return buf;
}
str formatDirectory(str dir, bool slash)
{
    if (!dir.empty())
    {
        const char& last = dir.back();
#ifdef _WIN32
        bool hasSlash = (last == '\\' || last == '/');
#else
        bool hasSlash = (last == '/');
#endif
        if (slash && !hasSlash) {
            dir.push_back('/');
        }
        else if (!slash && hasSlash) {
            dir.pop_back();
        }
    }
    return dir;
}

i64 diskTotal(const char* path)
{
    if (!isFileDirectoryExisted(path)){
        return -1;
    }

#ifdef _WIN32
    // get full path
    char buf[MAX_PATH] = { 0 };
    auto n = GetFullPathName(path, sizeof(buf), buf, NULL);
    if (n <= 0) {
        return -1;
    }
    // get driver name
    char drive[8] = { 0 };
    _splitpath(buf, drive, NULL, NULL, NULL);
    // get total disk
    ULARGE_INTEGER dt = { 0 };
    if (!GetDiskFreeSpaceEx(drive, NULL, &dt, NULL)) {
        return -1;
    }
    return i64(dt.QuadPart);
#else
    struct statvfs st;
    memset(&st, 0, sizeof(st));
    if (statvfs(path, &st)) {
        return -1;
    }
    return (st.f_blocks * st.f_frsize);
#endif
}
i64 diskFree(const char* path)
{
    if (!isFileDirectoryExisted(path)){
        return -1;
    }

#ifdef _WIN32
    // get full path
    char buf[MAX_PATH] = { 0 };
    auto n = GetFullPathName(path, sizeof(buf), buf, NULL);
    if (n <= 0) {
        return -1;
    }
    // get driver name
    char drive[8] = { 0 };
    _splitpath(buf, drive, NULL, NULL, NULL);
    // get free disk
    ULARGE_INTEGER df = { 0 };
    if (!GetDiskFreeSpaceEx(drive, &df, NULL, NULL)) {
        return -1;
    }
    return i64(df.QuadPart);
#else
    struct statvfs st;
    memset(&st, 0, sizeof(st));
    if (statvfs(path, &st)) {
        return -1;
    }
    return (st.f_bavail * st.f_frsize);
#endif
}
i64 fileSize(const char* path)
{
#ifdef _WIN32
    FILE* fd = fopen(path, "rb");
    if (fd) {
        _fseeki64(fd, 0, SEEK_END);
        i64 sz = _ftelli64(fd);
        fclose(fd);
        return sz;
    }
    return -1;
#else
    FILE* fd = fopen64(path, "rb");
    if (fd) {
        fseeko64(fd, 0, SEEK_END);
        i64 sz = ftello64(fd);
        fclose(fd);
        return sz;
    }
    return -1;
#endif
}
i64 directorySizeW(
    const char* dir,
    u32 maxdepth,
    const wchar_t* regex)
{
    if (!isDirectoryExisted(dir)){
        return -1;
    }

    i64 dsz = 0;
    auto files = scandirW(dir, SCAN_FILE, maxdepth, false, regex);
    for (auto it = files.begin(); it != files.end(); ++it){
        auto fsz = fileSize(it->c_str());
        if (fsz > 0){
            dsz += fsz;
        }
    }

    return dsz;
}
i64 directorySize(
        const char* dir,
        u32 maxdepth,
        const char* regex)
{
    wstr wregex;
    if (regex) {
#ifdef _WIN32
        wregex = mbs2wstr(regex, CP_ACP);
#else
        wregex = mbs2wstr(regex);
#endif
    }

    auto retval = directorySizeW(dir, maxdepth, wregex.c_str());
    return retval;
}

std::vector<str> scandirW(
    const char* dir,
    u32 mode,
    u32 maxdepth,
    bool nameonly,
    const wchar_t* regex)
{
    std::vector<str> vfns;
    if(!isDirectoryExisted(dir)){
        printf("%s not exist\n",dir);
        return vfns;
    }

    str path(dir);
    if (path.back() != '\\' && path.back() != '/'){
        path.append("/");
    }

    std::unique_ptr<std::wregex> re;

#ifdef _WIN32

    if (regex){
        re.reset(new std::wregex(regex));
    }

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile((path+"*").c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return vfns;
    }

    do
    {
        bool found = false;
        const char* fn = &ffd.cFileName[0];
        str newpath = path + fn;

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (strcmp(fn, ".") && strcmp(fn, "..")) {
                found = ((mode & SCAN_DIR) != 0);
                if (maxdepth){
                    // travel subdir if needed
                    auto newfns = scandirW(newpath.c_str(), mode, maxdepth-1, nameonly, regex);
                    vfns.insert(vfns.end(), newfns.begin(), newfns.end());
                }
            }
        }
        else {
            found = (mode & SCAN_FILE);
        }

        if (found)
        {
            if (re)
            {
                wstr wfn = mbs2wstr(fn, CP_ACP);
                if (wfn.empty()){
                    continue;
                }
                bool matched = std::regex_match(wfn.c_str(), *re);
                if (!matched){
                    continue;
                }
            }
            vfns.push_back(nameonly ? fn : newpath);
        }
    } while (FindNextFile(hFind, &ffd));

    FindClose(hFind);

#else

    if (regex){
        re.reset(new std::wregex());
        re->imbue(std::locale(""));  // 使用系统默认locale
        re->assign(regex);
    }

    DIR* dp = opendir(path.c_str());
    if (!dp){
        return vfns;
    }

    dirent* dirp = NULL;
    while ((dirp = readdir(dp)) != NULL)
    {
        bool found = false;
        struct stat st;
        const char* fn = &dirp->d_name[0];
        str newpath = path + fn;
        //printf("newpath = %s\n",newpath);
        if(dirp->d_type == DT_UNKNOWN)
        {
            /*Unix调用readdir当前只完全支持Btrfs, ext2, ext3, and ext4格式,当文件为其他格式时,可能出现d_type为0
             * 此时需使用其他方法判断文件类型
            */
            int iRet = stat(newpath.c_str(), &st);
            if(iRet != -1)
            {
                if(S_ISREG(st.st_mode))
                {
                    dirp->d_type = DT_REG;
                }
                else if(S_ISDIR(st.st_mode)){
                    dirp->d_type = DT_DIR;
                }
            }
        }
        if (dirp->d_type == DT_DIR)
        {
            if (strcmp(fn, ".") && strcmp(fn, "..")) {
                found = (mode & SCAN_DIR);
                if (maxdepth){
                    // travel subdir if needed
                    auto newfns = scandirW(newpath.c_str(), mode, maxdepth-1, nameonly, regex);
                    vfns.insert(vfns.end(), newfns.begin(), newfns.end());
                }
            }
        }
        else if (dirp->d_type == DT_REG){
            found = (mode & SCAN_FILE);
        }
        else {
            continue;
        }

        if (found)
        {
            //找到了一个普通文件符
            if (re)
            {
                wstr wfn = mbs2wstr(fn);
                if (wfn.empty()){
                    continue;
                }
                //进行正则匹配
                bool matched = std::regex_match(wfn.c_str(), *re);
                if (!matched){
                    continue;
                }
            }
            vfns.push_back(nameonly ? fn : newpath);
        }
    }
    closedir(dp);
#endif
    return vfns;
}
std::vector<str> scandir(
    const char* dir,
    u32 mode,
    u32 maxdepth,
    bool nameonly,
    const char* regex)
{
    wstr wregex;
    try{
    if (regex) {
#ifdef _WIN32
        wregex = mbs2wstr(regex, CP_ACP);
#else
        wregex = mbs2wstr(regex);
#endif
    }

    auto retval = scandirW(dir, mode, maxdepth, nameonly, wregex.c_str());

    return retval;
    }
    catch(std::regex_error& e)
    {
        printf("Exception regex_error : %s",e.what());
    }

}


ECCS_END
