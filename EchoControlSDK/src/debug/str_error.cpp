
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
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
        );

    if (lpMsgBuf){
        estr = (LPTSTR)lpMsgBuf;
        LocalFree(lpMsgBuf);
    }
#else
    char buf[64] = {0};
    estr = strerror_r(error, buf, sizeof(buf));
#endif

    if (estr.empty()){
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "Unknow error(%d)", error);
        estr = buf;
    }
    return estr;
}

ECCS_END
