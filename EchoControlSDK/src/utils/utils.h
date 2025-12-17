
#pragma once
#ifdef linux
#include <unistd.h>         // for getpid and usleep
#include <sys/sysinfo.h>    // for get_nprocs
#endif
#include <assert.h>
#include <math.h>
#include <time.h>
#include <vector>
#include "../global.h"
#include "../time/time_utils.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef MIN
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define DELETE_POINTER(p) \
    if (p) { \
        delete p; \
        p = NULL; \
    }

ECCS_BEGIN
// 内存信息的结构体，读文件/proc/meminfo
struct MemInfo {
    unsigned long total;        // MemTotal 总内存
    unsigned long free;         // MemFree  空闲内存
    unsigned long avaliable;    // MemAvailable 可用内存，约等于 MemFree + Buffer + Catch
};
//------------------------------------------------------
// flightbase
//------------------------------------------------------
//extern double round(double);

//------------------------------------------------------
// OS shutdown/reboot
//------------------------------------------------------

 void OsShutdown(u32 sec = 0);
 void OsReboot(u32 sec = 0);
 /**
  * @brief mySystem : 通过制定的call_type ,调用系统shell执行shell命令,并获取返回
  * @param command  : shell指令
  * @param print_output : 是否打印日志信息
  * @param call_type: 调用的类型 1:正常调用,2:获得shell的输出,存于output中
  * @param output : 若call_type = 2 , 则用于保存shell的输出
  * @return : 如果调用正常,返回空字符串,若调用失败,返回失败描述
  */
 std::string mySystem(const char *command,bool print_output = true,int call_type = 1,char* output = NULL);


 /**
  * @brief GetSystemMeminfo : 解析/proc/meminfo文件，获取内存信息
  * @details:MemTotal %lu
  *          MemFree %lu
  *          MemAvailable %lu (since Linux 3.14)
  * @param info
  */
void GetSystemMeminfo(MemInfo *info);
 /**
  * @brief GetProcessMeminfo : 解析/proc/{pid}/statm文件，获取对应进程内存信息
  * @details: size (pages) 任务虚拟地址空间的大小 VmSize/4，单位 KB
  *           resident(pages) 应用程序正在使用的物理内存的大小 VmRSS/4，单位 KB
  * @param pid : 进程号
  * @return 内存占用值
  */
unsigned long GetProcessMeminfo(int pid);

/**
 * @brief GetProcessMemUsed : 获取本进程已使用的内存，单位KB
 * @return
 */
int GetProcessMemUsed(void);
/**
 * @brief GetSystemMemUsed : 获取系统已使用的内存，单位MB。total为输出参数，表示总内存
 * @param total
 * @return
 */
int GetSystemMemUsed(int &total);


//------------------------------------------------------
// String utilities
//------------------------------------------------------

 std::vector<str> split(const str& s, char sep);
 std::vector<str> split(const str& s, const str& sep);

/// wrapper of sprintf
template<class T, size_t MAX_SIZE>
str toString(const T& val, const char* fmt)
{
    char buf[MAX_SIZE] = {0};
    sprintf(buf, fmt, val);
    return buf;
}
template<class T>
str toString(const T& val, const char* fmt)
{
    return toString<T, 32>(val, fmt);
}


//------------------------------------------------------
// Endian
//------------------------------------------------------

template <size_t N>
inline void copyBE(char* dst, const char* src)
{
    *dst = *(src + N - 1);
    copyBE<N - 1>(++dst, src);
}

template <>
inline void copyBE<size_t(1)>(char* dst, const char* src)
{
    *dst = *src;
}

template<typename T>
inline T typeConvertBE(const void* p)
{
    T t;
    copyBE<sizeof(T)>((char*)(&t), (char*)p);
    return t;
}

template <class T>
inline T typeConvert(const void* ptr)
{
    if ((size_t)ptr % sizeof(T) == 0) {
        return *(T*)(ptr);
    }
    else {
        T t;
        memcpy(&t, ptr, sizeof(T));
        return t;
    }
}


//------------------------------------------------------
// Math utilities
//------------------------------------------------------

inline double deg2rad(const double& deg)
{
    static const double factor = M_PI / 180.0;
    return (deg*factor);
}
 void formatangle(double& deg);           // deg -> [0, 360]
 void formatangle(float& deg);            // deg -> [0, 360]
 double angledelta(double a0, double a1); // degree

inline float round(const float& f)
{
    return (f > 0.0f) ? floor(f + 0.5f) : ceil(f - 0.5f);
}
inline double round(const double& d)
{
    return (d > 0.0) ? floor(d + 0.5) : ceil(d - 0.5);
}



//------------------------------------------------------
// Float compare
//------------------------------------------------------

template<class FloatType>
bool fequal(const FloatType& d1, const FloatType& d2, const FloatType& epsilon) // ==
{
    return (abs(d1 - d2) < epsilon);
}

template<class FloatType>
bool fge(const FloatType& d1, const FloatType& d2, const FloatType& epsilon)    // >=
{
    FloatType d = d1 - d2;
    if (abs(d) < epsilon) {
        return true;
    }
    return (d > 0);
}

template<class FloatType>
bool fle(const FloatType& d1, const FloatType& d2, const FloatType& epsilon)    // <=
{
    FloatType d = d1 - d2;
    if (abs(d) < epsilon) {
        return true;
    }
    return (d < 0);
}

template<class FloatType>
bool fgreater(const FloatType& d1, const FloatType& d2, const FloatType& epsilon)// >
{
    FloatType d = d1 - d2;
    if (abs(d) < epsilon) {
        return false;
    }
    return (d > 0);
}

template<class FloatType>
bool fless(const FloatType& d1, const FloatType& d2, const FloatType& epsilon)	// <
{
    FloatType d = d1 - d2;
    if (abs(d) < epsilon) {
        return false;
    }
    return (d < 0);
}


//------------------------------------------------------
// 字符编码转换
//------------------------------------------------------

#ifdef _WIN32

//------------------------------------------------------
// Convert local multi-bytes to UTF-16 on Windows
//
// codepage:
//   CP_ACP(0)      - Default to ANSI code page
//   CP_UTF8(65001) - UTF-8
//   936	        - GB2312
//   20936	        - x-cp20936	Simplified Chinese (GB2312); Chinese Simplified (GB2312-80)
//
// NOTE: Callers take owership of returned string
//------------------------------------------------------
wchar_t* mbs2ws(const char* mbs, int codepage = 0);
char*    ws2mbs(const wchar_t* ws, int codepage = 0);

wstr mbs2wstr(const char* mbs, int codepage = 0);
str  ws2mbstr(const wchar_t* ws, int codepage = 0);

#else

//------------------------------------------------------
// Convert UTF-8 to UTF-32 on linux
//
// NOTE: 1. Set proper C locale before mbs2ws() and ws2mbs():
//          * std::setlocale(LC_ALL, "xxx"): set C locale
//          * std::locale::global(std::locale("xxx")): set C and C++ locale
//          * SUGGESTION: using std::setlocale(LC_ALL, "xxx") in main() at startup
//                        don't using std::locale::global(), it will change all stream
//                        and std::wregex operations
//       2. Parameter 'mbs' and 'ws' must be end with '\0'
//       3. Callers take owership of returned string
//------------------------------------------------------
wchar_t* mbs2ws(const char* mbs);
char*    ws2mbs(const wchar_t* ws);

wstr mbs2wstr(const char* mbs);
str  ws2mbstr(const wchar_t* ws);

#endif


ECCS_END
