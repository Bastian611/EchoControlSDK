#ifndef ECHO_CTRL_CODE_H
#define ECHO_CTRL_CODE_H

#include <string>
#include "log/Logger.h"
#include "global.h"

ECCS_BEGIN

    // ----------------------------------
    // 通用返回码
    // ----------------------------------
    enum class ECReturnCode {
        ECCS_RET_SUCCESS = 0,                    // 操作成功
        ECCS_FAILURE,                        // 操作失败
        ECCS_PARTIAL_SUCCESS,                // 部分成功
        ECCS_NOT_INITIALIZED,                // 未初始化
        ECCS_IN_PROGRESS,                    // 操作中
        ECCS_UNSUPPORTED_OPERATION           // 不支持的操作
    };

    // ----------------------------------
    // 通用错误码
    // ----------------------------------
    enum class ECErrorCode {
        ECCS_SUCCESS = 0,                    // 操作成功
        ECCS_MEMORY_ALLOCATION_FAILED,       // 内存分配失败
        ECCS_INVALID_ARGUMENT,               // 无效的参数
        ECCS_UNKNOWN_ERROR                   // 未知错误
    };

    // ----------------------------------
    // 设备相关错误码
    // ----------------------------------
    enum class ECDeviceErrorCode {
        ECCS_DEVICE_NOT_FOUND = 1001,         // 未找到设备
        ECCS_DEVICE_INITIALIZATION_FAILED,    // 设备初始化失败
        ECCS_DEVICE_NOT_SUPPORTED,            // 设备不支持
        ECCS_DEVICE_BUSY,                     // 设备忙
        ECCS_DEVICE_DISCONNECTED              // 设备断开连接
    };

    // ----------------------------------
    // 配置相关错误码
    // ----------------------------------
    enum class ConfigErrorCode {
        ECCS_CONFIG_FILE_NOT_FOUND = 2001,    // 配置文件未找到
        ECCS_CONFIG_FILE_READ_FAILED,         // 配置文件读取失败
        ECCS_INVALID_CONFIG_FORMAT,           // 配置文件格式错误
        ECCS_MISSING_REQUIRED_PARAMETER       // 缺少必需的配置项
    };

    // ----------------------------------
    // 授权相关错误码
    // ----------------------------------
    enum class AuthorizationErrorCode {
        ECCS_LICENSE_EXPIRED = 3001,           // 授权过期
        ECCS_DEVICE_LIMIT_EXCEEDED,            // 超过设备授权数量
        ECCS_INVALID_LICENSE,                  // 无效的授权
        ECCS_NO_PERMISSION                     // 没有权限操作该设备
    };

    // ----------------------------------
    // 日志系统错误码
    // ----------------------------------
    enum class LoggingErrorCode {
        ECCS_LOG_FILE_WRITE_FAILED = 4001,     // 日志文件写入失败
        ECCS_LOG_LEVEL_NOT_SUPPORTED           // 不支持的日志级别
    };

    // ----------------------------------
    // 错误码对应的错误信息
    // ----------------------------------
    std::string getErrorMessage(ECErrorCode code) {
        switch (code) {
        case ECErrorCode::ECCS_SUCCESS:
            return "操作成功";
        case ECErrorCode::ECCS_MEMORY_ALLOCATION_FAILED:
            return "内存分配失败";
        case ECErrorCode::ECCS_INVALID_ARGUMENT:
            return "无效的参数";
        case ECErrorCode::ECCS_UNKNOWN_ERROR:
            return "未知错误";
        default:
            return "未定义错误";
        }
    }

    std::string getErrorMessage(ECDeviceErrorCode code) {
        switch (code) {
        case ECDeviceErrorCode::ECCS_DEVICE_NOT_FOUND:
            return "未找到设备";
        case ECDeviceErrorCode::ECCS_DEVICE_INITIALIZATION_FAILED:
            return "设备初始化失败";
        case ECDeviceErrorCode::ECCS_DEVICE_NOT_SUPPORTED:
            return "设备不支持";
        case ECDeviceErrorCode::ECCS_DEVICE_BUSY:
            return "设备忙";
        case ECDeviceErrorCode::ECCS_DEVICE_DISCONNECTED:
            return "设备断开连接";
        default:
            return "未定义设备错误";
        }
    }

    std::string getErrorMessage(ConfigErrorCode code) {
        switch (code) {
        case ConfigErrorCode::ECCS_CONFIG_FILE_NOT_FOUND:
            return "配置文件未找到";
        case ConfigErrorCode::ECCS_CONFIG_FILE_READ_FAILED:
            return "配置文件读取失败";
        case ConfigErrorCode::ECCS_INVALID_CONFIG_FORMAT:
            return "配置文件格式错误";
        case ConfigErrorCode::ECCS_MISSING_REQUIRED_PARAMETER:
            return "缺少必需的配置项";
        default:
            return "未定义配置错误";
        }
    }

    std::string getErrorMessage(AuthorizationErrorCode code) {
        switch (code) {
        case AuthorizationErrorCode::ECCS_LICENSE_EXPIRED:
            return "授权过期";
        case AuthorizationErrorCode::ECCS_DEVICE_LIMIT_EXCEEDED:
            return "超过设备授权数量";
        case AuthorizationErrorCode::ECCS_INVALID_LICENSE:
            return "无效的授权";
        case AuthorizationErrorCode::ECCS_NO_PERMISSION:
            return "没有权限操作该设备";
        default:
            return "未定义授权错误";
        }
    }

    std::string getErrorMessage(LoggingErrorCode code) {
        switch (code) {
        case LoggingErrorCode::ECCS_LOG_FILE_WRITE_FAILED:
            return "日志文件写入失败";
        case LoggingErrorCode::ECCS_LOG_LEVEL_NOT_SUPPORTED:
            return "不支持的日志级别";
        default:
            return "未定义日志错误";
        }
    }

    // ----------------------------------
    // 返回码对应的消息
    // ----------------------------------
    std::string getReturnCodeMessage(ECReturnCode code) {
        switch (code) {
        case ECReturnCode::ECCS_RET_SUCCESS:
            return "操作成功";
        case ECReturnCode::ECCS_FAILURE:
            return "操作失败";
        case ECReturnCode::ECCS_PARTIAL_SUCCESS:
            return "部分成功";
        case ECReturnCode::ECCS_NOT_INITIALIZED:
            return "未初始化";
        case ECReturnCode::ECCS_IN_PROGRESS:
            return "操作中";
        case ECReturnCode::ECCS_UNSUPPORTED_OPERATION:
            return "不支持的操作";
        default:
            return "未定义返回码";
        }
    }

#endif // ECHO_CTRL_CODE_H

ECCS_END
