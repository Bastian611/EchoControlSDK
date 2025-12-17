
#pragma once
#include <exception>
#include "../global.h"

#ifdef _WIN32
#define _E_NOEXCEPT
#else
#define _E_NOEXCEPT _GLIBCXX_USE_NOEXCEPT
#endif

ECCS_BEGIN


//------------------------------------------------------
// EOutOfBoundary
//------------------------------------------------------
/**
 * @brief The EOutOfBoundary class一般在数据越界时被抛出
 * @name :EOutOfBoundary异常类
 * @arg info：该异常的描述字符串
 * @arg idx ：越界时的位置计数值
 * @arg len ：数据存储数组的长度
 */
class EOutOfBoundary : public std::exception
{
public:
    EOutOfBoundary() _E_NOEXCEPT;
    EOutOfBoundary(const char* info) _E_NOEXCEPT;
    EOutOfBoundary(const char* info, size_t idx, size_t len) _E_NOEXCEPT;
    EOutOfBoundary(const EOutOfBoundary& other) _E_NOEXCEPT;
    EOutOfBoundary& operator=(const EOutOfBoundary& other) _E_NOEXCEPT;

    virtual const char* what() const _E_NOEXCEPT;

private:
    str _what;
};


//------------------------------------------------------
// ETypeCast
//------------------------------------------------------
/**
 * @brief The ETypeCast class一般在数据类型错误时被抛出
 * @name :ETypeCast异常类
 * @arg info：该异常的描述字符串
 */
class ETypeCast : public std::exception
{
public:
    ETypeCast() _E_NOEXCEPT;
    ETypeCast(const char* info) _E_NOEXCEPT;
    ETypeCast(const ETypeCast& other) _E_NOEXCEPT;
    ETypeCast& operator=(const ETypeCast& other) _E_NOEXCEPT;

    virtual const char* what() const _E_NOEXCEPT;

private:
    str _what;
};


//------------------------------------------------------
// EInvalidParam
//------------------------------------------------------
/**
 * @brief The EInvalidParam class一般在输入参数错误时被抛出
 * @name :EInvalidParam异常类
 * @arg fmt：该异常的描述字符串
 */
class EInvalidParam : public std::exception
{
public:
    EInvalidParam() _E_NOEXCEPT;
    EInvalidParam(const char* fmt, ...) _E_NOEXCEPT;
    EInvalidParam(const EInvalidParam& other) _E_NOEXCEPT;
    EInvalidParam& operator=(const EInvalidParam& other) _E_NOEXCEPT;

    virtual const char* what() const _E_NOEXCEPT;

private:
    str _what;
};


//------------------------------------------------------
// EInvalidOperation
//------------------------------------------------------
/**
 * @brief The EInvalidOperation class一般在执行了错误/未定义的操作时被抛出
 * @name :EInvalidOperation 异常类
 * @arg fmt：该异常的描述字符串
 */
class EInvalidOperation : public std::exception
{
public:
    EInvalidOperation() _E_NOEXCEPT;
    EInvalidOperation(const char* fmt, ...) _E_NOEXCEPT;
    EInvalidOperation(const EInvalidOperation& other) _E_NOEXCEPT;
    EInvalidOperation& operator=(const EInvalidOperation& other) _E_NOEXCEPT;

    virtual const char* what() const _E_NOEXCEPT;

private:
    str _what;
};


//------------------------------------------------------
// ESystemError
//------------------------------------------------------
/**
 * @brief The ESystemError class一般在系统进行文件/串口读写出错时被抛出
 * @name :ESystemError 异常类
 * @arg info：该异常的描述字符串
 * @arg error:系统返回的错误码
 */
class ESystemError : public std::exception
{
public:
    ESystemError() _E_NOEXCEPT;
    ESystemError(int error) _E_NOEXCEPT;
    ESystemError(const char* info) _E_NOEXCEPT;
    ESystemError(const char* info,int error) _E_NOEXCEPT;
    ESystemError(const ESystemError& other) _E_NOEXCEPT;
    ESystemError& operator=(const ESystemError& other) _E_NOEXCEPT;

    virtual const char* what() const _E_NOEXCEPT;

private:
    str _what;
};


//------------------------------------------------------
// ESocketError
//------------------------------------------------------
/**
 * @brief The ESocketError class一般在网络通信套接字出错时被抛出
 * @name :ESocketError 异常类
 * @arg info：该异常的描述字符串
 * @arg error:系统返回的错误码
 */
class ESocketError : public std::exception
{
public:
    ESocketError() _E_NOEXCEPT;
    ESocketError(int error) _E_NOEXCEPT;
    ESocketError(const char* info) _E_NOEXCEPT;
    ESocketError(const char* info, int error) _E_NOEXCEPT;
    ESocketError(const ESocketError& other) _E_NOEXCEPT;
    ESocketError& operator=(const ESocketError& other) _E_NOEXCEPT;

    virtual const char* what() const _E_NOEXCEPT;

private:
    str _what;
};


//------------------------------------------------------
// ETimeout
//------------------------------------------------------
/**
 * @brief The ETimeout class一般在操作超时时被抛出
 * @name :ETimeout 异常类
 * @arg info：该异常的描述字符串
 */
class ETimeout : public std::exception
{
public:
    ETimeout() _E_NOEXCEPT;
    ETimeout(const char* info) _E_NOEXCEPT;
    ETimeout(const ETimeout& other) _E_NOEXCEPT;
    ETimeout& operator=(const ETimeout& other) _E_NOEXCEPT;

    virtual const char* what() const _E_NOEXCEPT;

private:
    str _what;
};


//------------------------------------------------------
// EIOException
//------------------------------------------------------
/**
 * @brief The EIOException class一般在IO异常时被抛出
 * @name :EIOException 异常类
 * @arg fmt：该异常的描述字符串
 */
class EIOException : public std::exception
{
public:
    EIOException() _E_NOEXCEPT;
    EIOException(const char* fmt, ...) _E_NOEXCEPT;
    EIOException(const EIOException& other) _E_NOEXCEPT;
    EIOException& operator=(const EIOException& other) _E_NOEXCEPT;

    virtual const char* what() const _E_NOEXCEPT;

private:
    str _what;
};


//------------------------------------------------------
// EInterrupt
//------------------------------------------------------
/**
 * @brief The EInterrupt class一般在程序被异常中断，标志位状态变化时被抛出
 * @name :EInterrupt 异常类
 * @arg fmt：该异常的描述字符串
 */
class EInterrupt : public std::exception
{
public:
    EInterrupt() _E_NOEXCEPT;
    EInterrupt(const char* fmt, ...) _E_NOEXCEPT;
    EInterrupt(const EInterrupt& other) _E_NOEXCEPT;
    EInterrupt& operator=(const EInterrupt& other) _E_NOEXCEPT;

    virtual const char* what() const _E_NOEXCEPT;

private:
    str _what;
};
//------------------------------------------------------
// EDjiMopChannel
//------------------------------------------------------
/**
 * @brief The EDjiMopChannel class在大疆MOP通道传输异常时被抛出
 * @name :EInterrupt 异常类
 * @arg fmt：该异常的描述字符串
 */
class EDjiMopChannel : public std::exception
{
public:
    EDjiMopChannel() _E_NOEXCEPT;
    EDjiMopChannel(const char* str, ...) _E_NOEXCEPT;
    EDjiMopChannel(const EDjiMopChannel& other) _E_NOEXCEPT;
    EDjiMopChannel& operator=(const EDjiMopChannel& other) _E_NOEXCEPT;

    virtual const char* what() const _E_NOEXCEPT;

private:
    str _what;
};
//------------------------------------------------------
// EDjiMopChannelDisconnect
//------------------------------------------------------
/**
 * @brief The EDjiMopChannelDisconnect class在大疆MOP通道连接断开时被抛出
 * @name :EInterrupt 异常类
 * @arg fmt：该异常的描述字符串
 */
class EDjiMopChannelDisconnect : public std::exception
{
public:
    EDjiMopChannelDisconnect() _E_NOEXCEPT;
    EDjiMopChannelDisconnect(const char* str, ...) _E_NOEXCEPT;
    EDjiMopChannelDisconnect(const EDjiMopChannelDisconnect& other) _E_NOEXCEPT;
    EDjiMopChannelDisconnect& operator=(const EDjiMopChannelDisconnect& other) _E_NOEXCEPT;

    virtual const char* what() const _E_NOEXCEPT;

private:
    str _what;
};

ECCS_END