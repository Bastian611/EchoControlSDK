#ifndef ECHO_CTRL_CODE_H
#define ECHO_CTRL_CODE_H

#include <string>

// ----------------------------------
// 通用返回码
// ----------------------------------
enum ECCS_ReturnCode {
    ECCS_RET_SUCCESS = 0,
    ECCS_FAILURE,
    ECCS_PARTIAL_SUCCESS,
    ECCS_NOT_INITIALIZED,
    ECCS_IN_PROGRESS,
    ECCS_UNSUPPORTED_OPERATION
};

// ----------------------------------
// 通用错误码
// ----------------------------------
enum ECCS_ErrorCode {
    ECCS_SUCCESS = 0,
    ECCS_MEMORY_ALLOCATION_FAILED,
    ECCS_INVALID_ARGUMENT,
    ECCS_UNKNOWN_ERROR
};

// ----------------------------------
// 设备相关错误码
// ----------------------------------
enum ECCS_DeviceErrorCode {
    ECCS_DEVICE_NOT_FOUND = 1001,
    ECCS_DEVICE_INITIALIZATION_FAILED,
    ECCS_DEVICE_NOT_SUPPORTED,
    ECCS_DEVICE_BUSY,
    ECCS_DEVICE_DISCONNECTED
};

// ----------------------------------
// 配置相关错误码
// ----------------------------------
enum ECCS_ConfigErrorCode {
    ECCS_CONFIG_FILE_NOT_FOUND = 2001,
    ECCS_CONFIG_FILE_READ_FAILED,
    ECCS_INVALID_CONFIG_FORMAT,
    ECCS_MISSING_REQUIRED_PARAMETER
};

// ----------------------------------
// 授权相关错误码
// ----------------------------------
enum ECCS_AuthorizationErrorCode {
    ECCS_LICENSE_EXPIRED = 3001,
    ECCS_DEVICE_LIMIT_EXCEEDED,
    ECCS_INVALID_LICENSE,
    ECCS_NO_PERMISSION
};

// ----------------------------------
// 日志系统错误码
// ----------------------------------
enum ECCS_LoggingErrorCode {
    ECCS_LOG_FILE_WRITE_FAILED = 4001,
    ECCS_LOG_LEVEL_NOT_SUPPORTED
};

// ----------------------------------
// 回调事件类型
// ----------------------------------
enum ECCS_EventType {
    ECCS_EVT_UNKNOWN = 0,
    ECCS_EVT_STATUS_CHANGE = 1, // 设备状态变更 (上线/下线/故障) -> 数据为 DeviceStatus 结构
    ECCS_EVT_PTZ_REPORT = 2, // 云台角度回传 -> 数据为 PtzPosition 结构
    ECCS_EVT_SOUND_END = 3  // 播放结束通知
};

// ----------------------------------
// 设备类型定义
// ----------------------------------
enum ECCS_DevType {
    ECCS_DEV_UNKNOWN = 0,
    ECCS_DEV_LIGHT = 1,
    ECCS_DEV_SOUND = 2,
    ECCS_DEV_PTZ = 3,
    ECCS_DEV_RELAY = 4,
    ECCS_DEV_CAMERA = 5
};

// ----------------------------------
// 错误码对应的错误信息
// ----------------------------------

inline std::string getErrorMessage(ECCS_ErrorCode code) {
    switch (code) {
    case ECCS_ErrorCode::ECCS_SUCCESS:
        return "操作成功";
    case ECCS_ErrorCode::ECCS_MEMORY_ALLOCATION_FAILED:
        return "内存分配失败";
    case ECCS_ErrorCode::ECCS_INVALID_ARGUMENT:
        return "无效的参数";
    case ECCS_ErrorCode::ECCS_UNKNOWN_ERROR:
        return "未知错误";
    default:
        return "未定义错误";
    }
}

inline std::string getErrorMessage(ECCS_DeviceErrorCode code) {
    switch (code) {
    case ECCS_DeviceErrorCode::ECCS_DEVICE_NOT_FOUND:
        return "未找到设备";
    case ECCS_DeviceErrorCode::ECCS_DEVICE_INITIALIZATION_FAILED:
        return "设备初始化失败";
    case ECCS_DeviceErrorCode::ECCS_DEVICE_NOT_SUPPORTED:
        return "设备不支持";
    case ECCS_DeviceErrorCode::ECCS_DEVICE_BUSY:
        return "设备忙";
    case ECCS_DeviceErrorCode::ECCS_DEVICE_DISCONNECTED:
        return "设备断开连接";
    default:
        return "未定义设备错误";
    }
}

inline std::string getErrorMessage(ECCS_ConfigErrorCode code) {
    switch (code) {
    case ECCS_ConfigErrorCode::ECCS_CONFIG_FILE_NOT_FOUND:
        return "配置文件未找到";
    case ECCS_ConfigErrorCode::ECCS_CONFIG_FILE_READ_FAILED:
        return "配置文件读取失败";
    case ECCS_ConfigErrorCode::ECCS_INVALID_CONFIG_FORMAT:
        return "配置文件格式错误";
    case ECCS_ConfigErrorCode::ECCS_MISSING_REQUIRED_PARAMETER:
        return "缺少必需的配置项";
    default:
        return "未定义配置错误";
    }
}

inline std::string getErrorMessage(ECCS_AuthorizationErrorCode code) {
    switch (code) {
    case ECCS_AuthorizationErrorCode::ECCS_LICENSE_EXPIRED:
        return "授权过期";
    case ECCS_AuthorizationErrorCode::ECCS_DEVICE_LIMIT_EXCEEDED:
        return "超过设备授权数量";
    case ECCS_AuthorizationErrorCode::ECCS_INVALID_LICENSE:
        return "无效的授权";
    case ECCS_AuthorizationErrorCode::ECCS_NO_PERMISSION:
        return "没有权限操作该设备";
    default:
        return "未定义授权错误";
    }
}

inline std::string getErrorMessage(ECCS_LoggingErrorCode code) {
    switch (code) {
    case ECCS_LoggingErrorCode::ECCS_LOG_FILE_WRITE_FAILED:
        return "日志文件写入失败";
    case ECCS_LoggingErrorCode::ECCS_LOG_LEVEL_NOT_SUPPORTED:
        return "不支持的日志级别";
    default:
        return "未定义日志错误";
    }
}

inline std::string getReturnCodeMessage(ECCS_ReturnCode code) {
    switch (code) {
    case ECCS_ReturnCode::ECCS_RET_SUCCESS:
        return "操作成功";
    case ECCS_ReturnCode::ECCS_FAILURE:
        return "操作失败";
    case ECCS_ReturnCode::ECCS_PARTIAL_SUCCESS:
        return "部分成功";
    case ECCS_ReturnCode::ECCS_NOT_INITIALIZED:
        return "未初始化";
    case ECCS_ReturnCode::ECCS_IN_PROGRESS:
        return "操作中";
    case ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION:
        return "不支持的操作";
    default:
        return "未定义返回码";
    }
}

#endif