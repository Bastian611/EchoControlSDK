
#include "utils.h"
#include <assert.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <string.h>
#include <locale>
#endif
#include "../thread/sal_thread.h"
#include "../time/sal_chrono.h"

ECCS_BEGIN
//------------------------------------------------------
// flightbase
//------------------------------------------------------
//inline double round(double num)
//{
//    return (num > 0.0) ? floor(num + 0.5) : ceil(num - 0.5);
//}

//------------------------------------------------------
// OS shutdown/reboot
//------------------------------------------------------

void OsShutdown(u32 sec)
{
#ifdef _WIN32
    str cmd("shutdown -s -t " + std::to_string((long long)sec));
    system(cmd.c_str());
#else
    str cmd("( sleep " + std::to_string((long long)sec) + " ; sudo shutdown -h now ) &");
    system(cmd.c_str());
#endif
}
void OsReboot(u32 sec)
{
#ifdef _WIN32
    str cmd("shutdown -r -t " + std::to_string((long long)sec));
    system(cmd.c_str());
#else
    str cmd("( sleep " + std::to_string((long long)sec) + " ; sudo shutdown -r now ) &");
    system(cmd.c_str());
#endif
}
std::string mySystem(const char *command,bool print_output,int call_type,char* output)
{
#if __linux
    int32_t ret;
    FILE* shell_rstream;
    char shell_ret[256];
    if(call_type == 1)
    {
        ret = system(command);
        if(ret == -1)
        {
            //system fork子线程失败
            sprintf(shell_ret,"system() failed : %s",strerror(errno));
            if(print_output)
            LOG_ERROR("%s",shell_ret);
            return std::string(shell_ret);
        }
        if(!WIFEXITED(ret))
        {
            //system子线程无法执行shell
            sprintf(shell_ret,"can not excecute system call [%s]",command);
            if(print_output)
            LOG_ERROR("%s",shell_ret);
            return std::string(shell_ret);
        }
        char exit_status = WEXITSTATUS(ret);
        if(ret != 0)
        {
            sprintf(shell_ret,"system call [%s] failed : %s\n",command,strerror(exit_status));
            if(print_output)
            LOG_ERROR("%s",shell_ret);
            return std::string(shell_ret);
        }
        if(print_output)
        LOG_DEBUG("system call [%s] ret : %x",command,exit_status);
        return "";
    }
    else if(call_type == 2)
    {
        if(output == NULL)
        {
            return "Param Error : output is NULL";
        }

        shell_rstream = popen(command,"r");
        if(NULL == shell_rstream)
        {
            str ret("mySystem ");
            ret += command;
            ret += "Call Failed";
            return ret;
        }
        fread(output,sizeof(char),255,shell_rstream);
        if(print_output)
        LOG_INFO("cmd : %s \n \r ret : %s",command,output);
        ret = pclose(shell_rstream);
        char exit_status = WEXITSTATUS(ret);
        if(ret != 0)
        {
            sprintf(shell_ret,"system call [%s] failed : %s\n",command,strerror(exit_status));
            if(print_output)
            LOG_ERROR("%s",shell_ret);
            return std::string(shell_ret);
        }
        return "";
    }
    else
    {
        return "Error call_type";
    }
#else
    return "Error Platform";
#endif
}
 /**
  * @brief GetSystemMeminfo : 解析/proc/meminfo文件，获取内存信息
  * @details:MemTotal %lu KB
  *          MemFree %lu KB
  *          MemAvailable %lu (since Linux 3.14)
  * @param info
  */
void GetSystemMeminfo(MemInfo *info) {
    memset(info, 0, sizeof(MemInfo));
#ifdef __linux
    char buf[128];
    char name[64];
    char unit[64];
    FILE *fd = fopen("/proc/meminfo", "r");
    assert(fd != nullptr);
    fgets(buf, sizeof(buf), fd);
    sscanf(buf, "%s %u %s", name, &info->total, unit);
    fgets(buf, sizeof(buf), fd);
    sscanf(buf, "%s %u %s", name, &info->free, unit);
    fgets(buf, sizeof(buf), fd);
    sscanf(buf, "%s %u %s", name, &info->avaliable, unit);
    fclose(fd);
    // fprintf(stdout, "mem: %u %u %u\n", info->total, info->free, info->avaliable);
#endif
}
 /**
  * @brief GetProcessMeminfo : 解析/proc/{pid}/statm文件，获取对应进程内存信息
  * @details: size (pages) 任务虚拟地址空间的大小 VmSize/4，单位 KB
  *           resident(pages) 应用程序正在使用的物理内存的大小，单位为PAGESIZE
  * @param pid : 进程号
  * @return 内存占用值，单位 KB
  */
unsigned long GetProcessMeminfo(int pid) {
#ifdef __linux
    // 计算当前进程对应的文件名
    int sys_pagesize = getpagesize();
    char filename[64];
    snprintf(filename, sizeof(filename) - 1, "/proc/%d/statm", pid);

    char buf[256];
    FILE *fd = fopen(filename, "r");
    assert(fd != nullptr);
    fgets(buf, sizeof(buf), fd);
    fclose(fd);

    unsigned long size = 0;
    unsigned long resident = 0;
    sscanf(buf, "%lu %lu", &size, &resident);
    //fprintf(stdout, "proc mem: %u %u pagesize %d\n", size, resident,sys_pagesize);
    return (resident * sys_pagesize)/1024;
#else
    return -1;
#endif
}

/**
 * @brief GetProcessMemUsed : 获取本进程已使用的内存，单位KB
 * @return
 */
int GetProcessMemUsed(void) {
#ifdef __linux
    unsigned long mem_used = GetProcessMeminfo(getpid());
    return mem_used;
#else
    return -1;
#endif
}
/**
 * @brief GetSystemMemUsed : 获取系统已使用的内存，单位MB。total为输出参数，表示总内存
 * @param total
 * @return
 */
int GetSystemMemUsed(int &total) {
#ifdef __linux
    MemInfo info;
    GetSystemMeminfo(&info);
    total = info.total / 1024;
    return (total - info.avaliable / 1024);
#else
    return -1;
#endif
}

//------------------------------------------------------
// String utilities
//------------------------------------------------------

std::vector<str> split(const str& s, char sep)
{
    std::vector<str> vStr;

    if (s.empty()) {
        return vStr;
    }

    size_t len = s.length(), pos = 0;
    for (size_t i = 0; i < len; ++i)
    {
        if (s[i] == sep) {
            if (i > pos) {
                vStr.push_back(s.substr(pos, i - pos));
            }
            pos = i + 1;
        }
    }

    if (pos < len) {
        vStr.push_back(s.substr(pos, len - pos));
    }

    return vStr;
}
std::vector<str> split(const str& s, const str& sep)
{
    std::vector<str> vStr;

    if (sep.empty() || s.empty()) {
        return vStr;
    }

    size_t len0 = sep.length(), len1 = s.length();
    if (len1 < len0) {
        vStr.push_back(s);
        return vStr;
    }

    size_t pos = 0, start = 0;
    for (size_t i = len0 - 1; i < len1; ++i)
    {
        bool match = true;
        start = i - len0 + 1;
        for (size_t j = 0; j < len0; ++j) {
            if (s[start + j] != sep[j]) {
                match = false;
                break;
            }
        }

        if (match) {
            if (start > pos) {
                vStr.push_back(s.substr(pos, start - pos));
            }
            pos = start + len0;
        }
    }

    if (pos < len1) {
        vStr.push_back(s.substr(pos, len1 - pos));
    }

    return vStr;
}


//------------------------------------------------------
// Math utilities
//------------------------------------------------------

void formatangle(double& deg)
{
    while(deg < 0){
        deg += 360;
    }
    while(deg > 360){
        deg -= 360;
    }
}
void formatangle(float& deg)
{
    while(deg < 0){
        deg += 360;
    }
    while(deg > 360){
        deg -= 360;
    }
}
double angledelta(double a0, double a1)
{
    // format angles
    formatangle(a0);
    formatangle(a1);

    // transform angles
    assert(a0 >= 0 && a0 <= 360);
    assert(a1 >= 0 && a1 <= 360);
    if (a0 - a1 > 180){
        a0 -= 360;
    }
    else if (a1 - a0 > 180){
        a1 -= 360;
    }

    assert(abs(a1-a0) <= 180);
    return (a0-a1);
}


//------------------------------------------------------
// 字符编码转换
//------------------------------------------------------

#ifdef _WIN32

wchar_t* mbs2ws(const char* mbs, int codepage)
{
    DWORD sz = MultiByteToWideChar(codepage, 0, mbs, -1, NULL, 0);
    if (sz == 0){
        return NULL;
    }
    wchar_t* ws = new wchar_t[sz];
    MultiByteToWideChar (codepage, 0, mbs, -1, ws, sz);
    return ws;
}
char* ws2mbs(const wchar_t* ws, int codepage)
{
    DWORD sz = WideCharToMultiByte(codepage, NULL, ws,-1, NULL, 0, NULL, FALSE);
    if (sz == 0){
        return NULL;
    }
    char* mbs = new char[sz];
    WideCharToMultiByte (codepage, NULL, ws, -1, mbs, sz, NULL, FALSE);
    return mbs;
}

wstr mbs2wstr(const char* mbs, int codepage)
{
    wstr ws;

    DWORD sz = MultiByteToWideChar(codepage, 0, mbs, -1, NULL, 0);
    if (sz == 0) {
        return ws;
    }

    ws.resize(sz);
    MultiByteToWideChar(codepage, 0, mbs, -1, (wchar_t*)(ws.c_str()), sz);
    if (ws.back() == 0) {  // the last char is '\0'
        ws.pop_back();
    }
    return ws;
}
str  ws2mbstr(const wchar_t* ws, int codepage)
{
    str mbs;

    DWORD sz = WideCharToMultiByte(codepage, NULL, ws,-1, NULL, 0, NULL, FALSE);
    if (sz == 0) {
        return mbs; // empty string
    }

    mbs.resize(sz);
    WideCharToMultiByte(codepage, NULL, ws, -1, (char*)(mbs.c_str()), sz, NULL, FALSE);
    if (mbs.back() == 0) { // the last char is '\0'
        mbs.pop_back();
    }
    return mbs;
}

#else

wchar_t* mbs2ws(const char* mbs)
{
    std::mbstate_t st = std::mbstate_t();
    int len = 1 + std::mbsrtowcs(NULL, &mbs, 0, &st);
    if (len > 1){
        wchar_t* ws = new wchar_t[len];
        memset(ws, 0, len*sizeof(wchar_t));
        std::mbsrtowcs(ws, &mbs, len, &st);
        return ws;
    }
    return NULL;
}
char* ws2mbs(const wchar_t* ws)
{
    std::mbstate_t st = std::mbstate_t();
    int len = 1 + std::wcsrtombs(NULL, &ws, 0, &st);
    if (len > 1){
        char* mbs = new char[len];
        memset(mbs, 0, len);
        std::wcsrtombs(mbs, &ws, len, &st);
        return mbs;
    }
    return NULL;
}

wstr mbs2wstr(const char* mbs)
{
    wstr ws;

    std::mbstate_t st = std::mbstate_t();
    int len = 1 + std::mbsrtowcs(NULL, &mbs, 0, &st);
    if (len > 1){
        ws.resize(len);
        memset((void*)(ws.c_str()), 0, len*sizeof(wchar_t));
        std::mbsrtowcs((wchar_t*)(ws.c_str()), &mbs, len, &st);
    }
    return ws;
}
str  ws2mbstr(const wchar_t* ws)
{
    str mbs;

    std::mbstate_t st = std::mbstate_t();
    int len = 1 + std::wcsrtombs(NULL, &ws, 0, &st);
    if (len > 1){
        mbs.resize(len);
        memset((void*)(mbs.c_str()), 0, len);
        std::wcsrtombs((char*)(mbs.c_str()), &ws, len, &st);
    }

    return mbs;
}

#endif


ECCS_END
