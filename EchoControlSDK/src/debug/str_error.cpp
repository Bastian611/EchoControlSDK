
#include "str_error.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <stdio.h>
#include <string.h>
#endif

ECCS_BEGIN


str strError(i32 error)
{
    str estr;

#ifdef _WIN32
    LPVOID lpMsgBuf = NULL;
    // [20251215修改] 强制使用 FormatMessageA (ANSI 版本)，确保返回 char*
    // 即使项目定义了 UNICODE，这里也能正确处理
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPSTR)&lpMsgBuf, // 强制转换为 LPSTR (char*)
        0,
        NULL
        );

    if (lpMsgBuf){
        estr = (LPTSTR)lpMsgBuf;
        LocalFree(lpMsgBuf);

        // 去除 Windows 错误信息末尾自带的换行符
        if (!estr.empty() && estr.back() == '\n') estr.pop_back();
        if (!estr.empty() && estr.back() == '\r') estr.pop_back();
    }
#else

    char buf[256] = { 0 };
    // [20251215修改] 兼容处理 strerror_r
    // 直接将返回值写到 buf 里，然后从 buf 读取
    // 这样无论 strerror_r 返回 int 还是 char*，代码都能编译通过且运行正确

#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
    // XSI-compliant version (returns int)
    strerror_r(error, buf, sizeof(buf));

#else
    // GNU-specific version (returns char*)
    // 有些版本如果不使用返回值可能会报警告，这里做一个简单的转换防止编译器报错
    const char* res = strerror_r(error, buf, sizeof(buf));
    (void)res;

#endif
    estr = buf; // 从 buffer 构造 string

#endif

    if (estr.empty()){
        char buf[64] = {0};
        snprintf(buf, sizeof(buf), "Unknow error(%d)", error);
        estr = buf;
    }
    return estr;
}

ECCS_END
