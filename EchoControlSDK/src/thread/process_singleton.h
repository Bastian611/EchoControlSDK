
#pragma once
#include "../global.h"

#ifdef _WIN32
#include <Windows.h>
#endif

ECCS_BEGIN


class ProcessSingleton
{
    NON_COPYABLE(ProcessSingleton);

public:
    ProcessSingleton(const char* name);
    ~ProcessSingleton();

    bool isGood() const { return _good; }
    bool isProcessExisted() const { return _existed; }

private:
    str    _name;
    bool   _good;
    bool   _existed;

#ifdef _WIN32
    HANDLE _mutex;
#else
    int    _fd;
#endif
};


ECCS_END
