
#include "Exceptions.h"
#include <stdarg.h>
#include <string.h>
#include "../global.h"
#include "../debug/str_error.h"

ECCS_BEGIN


//------------------------------------------------------
// EOutOfBoundary
//------------------------------------------------------

EOutOfBoundary::EOutOfBoundary() _E_NOEXCEPT
    : std::exception(), _what("Out of boundary")
{

}
EOutOfBoundary::EOutOfBoundary(const char* info) _E_NOEXCEPT
    : std::exception(), _what(info)
{

}
EOutOfBoundary::EOutOfBoundary(const char* info, size_t idx, size_t len) _E_NOEXCEPT
    : std::exception()
{
    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "Out of boundary(idx: %zd, len: %zd): %s", idx, len, info);
    _what = buf;
}
EOutOfBoundary::EOutOfBoundary(const EOutOfBoundary& other) _E_NOEXCEPT
    : std::exception(other), _what(other._what)
{

}
EOutOfBoundary& EOutOfBoundary::operator=(const EOutOfBoundary& other) _E_NOEXCEPT
{
    if (this != &other){
        std::exception::operator =(other);
        _what = other._what;
    }
    return (*this);
}
const char* EOutOfBoundary::what() const _E_NOEXCEPT
{
    return _what.c_str();
}


//------------------------------------------------------
// ETypeCast
//------------------------------------------------------

ETypeCast::ETypeCast() _E_NOEXCEPT
    : std::exception(), _what("Type Cast exception")
{

}
ETypeCast::ETypeCast(const char* info) _E_NOEXCEPT
    : std::exception(), _what(info)
{

}
ETypeCast::ETypeCast(const ETypeCast& other) _E_NOEXCEPT
    : std::exception(other), _what(other._what)
{

}
ETypeCast& ETypeCast::operator=(const ETypeCast& other) _E_NOEXCEPT
{
    if (this != &other){
        std::exception::operator =(other);
        _what = other._what;
    }
    return (*this);
}
const char* ETypeCast::what() const _E_NOEXCEPT
{
    return _what.c_str();
}


//------------------------------------------------------
// EInvalidParam
//------------------------------------------------------

EInvalidParam::EInvalidParam() _E_NOEXCEPT
    : std::exception(), _what("Invalid parameter")
{

}
EInvalidParam::EInvalidParam(const char* fmt, ...) _E_NOEXCEPT
    : std::exception()
{
    const static size_t BUF_LEN = 1024;

    char buf[BUF_LEN] = {0};
    va_list args;
    va_start(args, fmt);
    auto iRet = vsnprintf_s(buf, BUF_LEN-1, fmt, args);
    va_end(args);

    if (iRet >= 0){
        _what = buf;
    }
    else{
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "message to long: %s", fmt);
        _what = buf;
    }
}
EInvalidParam::EInvalidParam(const EInvalidParam& other) _E_NOEXCEPT
    : std::exception(other), _what(other._what)
{

}
EInvalidParam& EInvalidParam::operator=(const EInvalidParam& other) _E_NOEXCEPT
{
    if (this != &other){
        std::exception::operator =(other);
        _what = other._what;
    }
    return (*this);
}
const char* EInvalidParam::what() const _E_NOEXCEPT
{
    return _what.c_str();
}


//------------------------------------------------------
// EInvalidOperation
//------------------------------------------------------

EInvalidOperation::EInvalidOperation() _E_NOEXCEPT
    : std::exception(), _what("Invalid operation")
{

}
EInvalidOperation::EInvalidOperation(const char* fmt, ...) _E_NOEXCEPT
    : std::exception()
{
    const static size_t BUF_LEN = 1024;

    char buf[BUF_LEN] = { 0 };
    va_list args;
    va_start(args, fmt);
    auto iRet = vsnprintf_s(buf, BUF_LEN - 1, fmt, args);
    va_end(args);

    if (iRet >= 0) {
        _what = buf;
    }
    else {
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "message to long: %s", fmt);
        _what = buf;
    }
}
EInvalidOperation::EInvalidOperation(const EInvalidOperation& other) _E_NOEXCEPT
    : std::exception(other), _what(other._what)
{

}
EInvalidOperation& EInvalidOperation::operator=(const EInvalidOperation& other) _E_NOEXCEPT
{
    if (this != &other) {
        std::exception::operator =(other);
        _what = other._what;
    }
    return (*this);
}
const char* EInvalidOperation::what() const _E_NOEXCEPT
{
    return _what.c_str();
}


//------------------------------------------------------
// ESystemError
//------------------------------------------------------

ESystemError::ESystemError() _E_NOEXCEPT
    : std::exception(), _what("System Error")
{

}
ESystemError::ESystemError(int error) _E_NOEXCEPT
    : std::exception()
{
    _what = strError(error);
}
ESystemError::ESystemError(const char* info) _E_NOEXCEPT
    : std::exception(), _what(info)
{

}
ESystemError::ESystemError(const char* info,int error) _E_NOEXCEPT
    : std::exception(), _what(info)
{
    char buf[16] = { 0 };
    snprintf(buf, sizeof(buf), "%d", error);
    _what += ": [" + str(buf) + "] " + strError(error);
}
ESystemError::ESystemError(const ESystemError& other) _E_NOEXCEPT
    : std::exception(other), _what(other._what)
{

}
ESystemError& ESystemError::operator=(const ESystemError& other) _E_NOEXCEPT
{
    if (this != &other) {
        std::exception::operator =(other);
        _what = other._what;
    }
    return (*this);
}
const char* ESystemError::what() const _E_NOEXCEPT
{
    return _what.c_str();
}


//------------------------------------------------------
// ESocketError
//------------------------------------------------------

ESocketError::ESocketError() _E_NOEXCEPT
    : std::exception(), _what("Socket Error")
{

}
ESocketError::ESocketError(int error) _E_NOEXCEPT
    : std::exception()
{
    _what = strError(error);
}
ESocketError::ESocketError(const char* info) _E_NOEXCEPT
    : std::exception(), _what(info)
{

}
ESocketError::ESocketError(const char* info, int error) _E_NOEXCEPT
    : std::exception(), _what(info)
{
    char buf[16] = { 0 };
    snprintf(buf, sizeof(buf), "%d", error);
    _what += ": [" + str(buf) + "] " + strError(error);
}
ESocketError::ESocketError(const ESocketError& other) _E_NOEXCEPT
    : std::exception(other), _what(other._what)
{

}
ESocketError& ESocketError::operator=(const ESocketError& other) _E_NOEXCEPT
{
    if (this != &other){
        std::exception::operator =(other);
        _what = other._what;
    }
    return (*this);
}
const char* ESocketError::what() const _E_NOEXCEPT
{
    return _what.c_str();
}


//------------------------------------------------------
// ETimeout
//------------------------------------------------------

ETimeout::ETimeout() _E_NOEXCEPT
    : std::exception(), _what("Timeout")
{

}
ETimeout::ETimeout(const char* info) _E_NOEXCEPT
    : std::exception(), _what(info)
{

}
ETimeout::ETimeout(const ETimeout& other) _E_NOEXCEPT
    : std::exception(other), _what(other._what)
{

}
ETimeout& ETimeout::operator=(const ETimeout& other) _E_NOEXCEPT
{
    if (this != &other){
        std::exception::operator =(other);
        _what = other._what;
    }
    return (*this);
}
const char* ETimeout::what() const _E_NOEXCEPT
{
    return _what.c_str();
}


//------------------------------------------------------
// EIOException
//------------------------------------------------------

EIOException::EIOException() _E_NOEXCEPT
    : std::exception(), _what("IO Exception")
{

}
EIOException::EIOException(const char* fmt, ...) _E_NOEXCEPT
    : std::exception()
{
    const static size_t BUF_LEN = 1024;

    char buf[BUF_LEN] = {0};
    va_list args;
    va_start(args, fmt);
    auto iRet = vsnprintf_s(buf, BUF_LEN-1, fmt, args);
    va_end(args);

    if (iRet >= 0){
        _what = buf;
    }
    else{
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "IOException message to long: %s", fmt);
        _what = buf;
    }
}
EIOException::EIOException(const EIOException& other) _E_NOEXCEPT
    : std::exception(other), _what(other._what)
{

}
EIOException& EIOException::operator=(const EIOException& other) _E_NOEXCEPT
{
    if (this != &other){
        std::exception::operator =(other);
        _what = other._what;
    }
    return (*this);
}
const char* EIOException::what() const _E_NOEXCEPT
{
    return _what.c_str();
}


//------------------------------------------------------
// EInterrupt
//------------------------------------------------------

EInterrupt::EInterrupt() _E_NOEXCEPT
    : std::exception(), _what("Interrupt")
{

}
EInterrupt::EInterrupt(const char* fmt, ...) _E_NOEXCEPT
    : std::exception()
{
    const static size_t BUF_LEN = 1024;

    char buf[BUF_LEN] = {0};
    va_list args;
    va_start(args, fmt);
    auto iRet = vsnprintf_s(buf, BUF_LEN-1, fmt, args);
    va_end(args);

    if (iRet >= 0){
        _what = buf;
    }
    else{
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "Interrupt message to long: %s", fmt);
        _what = buf;
    }
}
EInterrupt::EInterrupt(const EInterrupt& other) _E_NOEXCEPT
    : std::exception(other), _what(other._what)
{

}
EInterrupt& EInterrupt::operator=(const EInterrupt& other) _E_NOEXCEPT
{
    if (this != &other){
        std::exception::operator =(other);
        _what = other._what;
    }
    return (*this);
}
const char* EInterrupt::what() const _E_NOEXCEPT
{
    return _what.c_str();
}
//------------------------------------------------------
// EDjiMopChannel
//------------------------------------------------------
EDjiMopChannel::EDjiMopChannel() _E_NOEXCEPT
    : std::exception(), _what("Interrupt")
{

}
EDjiMopChannel::EDjiMopChannel(const char* str, ...) _E_NOEXCEPT
    : std::exception()
{
    const static size_t BUF_LEN = 1024;

    char buf[BUF_LEN] = {0};
    va_list args;
    va_start(args, str);
    auto iRet = vsnprintf_s(buf, BUF_LEN-1, str, args);
    va_end(args);

    if (iRet >= 0){
        _what = buf;
    }
    else{
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "Interrupt message to long: %s", str);
        _what = buf;
    }
}
EDjiMopChannel::EDjiMopChannel(const EDjiMopChannel& other) _E_NOEXCEPT
    : std::exception(other), _what(other._what)
{

}
EDjiMopChannel& EDjiMopChannel::operator=(const EDjiMopChannel& other) _E_NOEXCEPT
{
    if (this != &other){
        std::exception::operator =(other);
        _what = other._what;
    }
    return (*this);
}
const char* EDjiMopChannel::what() const _E_NOEXCEPT
{
    return _what.c_str();
}
//------------------------------------------------------
// EDjiMopChannelDisconnect
//------------------------------------------------------
EDjiMopChannelDisconnect::EDjiMopChannelDisconnect() _E_NOEXCEPT
    : std::exception(), _what("Interrupt")
{

}
EDjiMopChannelDisconnect::EDjiMopChannelDisconnect(const char* str, ...) _E_NOEXCEPT
    : std::exception()
{
    const static size_t BUF_LEN = 1024;

    char buf[BUF_LEN] = {0};
    va_list args;
    va_start(args, str);
    auto iRet = vsnprintf_s(buf, BUF_LEN-1, str, args);
    va_end(args);

    if (iRet >= 0){
        _what = buf;
    }
    else{
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "Interrupt message to long: %s", str);
        _what = buf;
    }
}
EDjiMopChannelDisconnect::EDjiMopChannelDisconnect(const EDjiMopChannelDisconnect& other) _E_NOEXCEPT
    : std::exception(other), _what(other._what)
{

}
EDjiMopChannelDisconnect& EDjiMopChannelDisconnect::operator=(const EDjiMopChannelDisconnect& other) _E_NOEXCEPT
{
    if (this != &other){
        std::exception::operator =(other);
        _what = other._what;
    }
    return (*this);
}
const char* EDjiMopChannelDisconnect::what() const _E_NOEXCEPT
{
    return _what.c_str();
}
ECCS_END
