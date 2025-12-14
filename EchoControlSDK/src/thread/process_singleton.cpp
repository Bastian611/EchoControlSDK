
#include "process_singleton.h"
#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#endif

ECCS_BEGIN


ProcessSingleton::ProcessSingleton(const char* name) 
    : _name(name), _good(false), _existed(false)
{
    _good = false;
    _existed = false;
    
#ifdef _WIN32
    _mutex = CreateMutex(NULL, TRUE, _name.c_str());
    if (_mutex) {
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            _good = true;
            _existed = true;
        }
    }
    else { 
        _good = false;
        _existed = true;
    }
#else
    _name = "/tmp/" + _name;
    _fd = open(_name.c_str(), O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (_fd > 0)
    {
        struct flock lock;
        bzero(&lock, sizeof(lock));
        if (fcntl(_fd, F_GETLK, &lock) >= 0)
        {
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            if (fcntl(_fd, F_SETLK, &lock) < 0) {
                if (errno == EACCES || errno == EAGAIN){
                    _good = true;
                    _existed = true;
                }
            }
            else {
                _good = true;
                _existed = false;
            }
        }
    }
#endif
}
ProcessSingleton::~ProcessSingleton()
{
#ifdef _WIN32
    if (_mutex) {
        CloseHandle(_mutex);
        _mutex = NULL;
    }
#else
    if (_fd > 0) {
        close(_fd);
        _fd = -1;
    }
#endif
}


ECCS_END
