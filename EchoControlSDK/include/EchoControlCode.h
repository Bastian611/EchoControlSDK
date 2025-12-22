#ifndef ECHO_CTRL_CODE_H
#define ECHO_CTRL_CODE_H

#include <string>

// -----------------------------------------------------------
// 统一返回码定义 (ECCS_Error)
// -----------------------------------------------------------
enum ECCS_Error {
    // === 成功 ===
    ECCS_SUCCESS = 0,  // 操作成功

    // === 通用错误 (1 - 99) ===
    ECCS_ERR_FAILED = 1,  // 通用失败
    ECCS_ERR_INVALID_PARAM = 2,  // 参数无效 (空指针、范围错误)
    ECCS_ERR_NOT_INIT = 3,  // SDK 未初始化
    ECCS_ERR_NOT_SUPPORTED = 4,  // 不支持该操作
    ECCS_ERR_TIMEOUT = 5,  // 操作超时
    ECCS_ERR_MALLOC = 6,  // 内存分配失败

    // === 设备相关 (100 - 199) ===
    ECCS_ERR_DEV_NOT_FOUND = 100, // 句柄无效或设备未找到
    ECCS_ERR_DEV_OFFLINE = 101, // 设备不在线
    ECCS_ERR_DEV_BUSY = 102, // 设备忙/正在执行
    ECCS_ERR_DEV_TYPE_MISMATCH = 103, // 设备类型不匹配 (如给灯发云台指令)
    ECCS_ERR_DEV_SEND_FAILED = 104, // 网络/串口发送失败

    // === 配置与文件 (200 - 299) ===
    ECCS_ERR_CFG_LOAD_FAILED = 200, // 配置文件加载失败 (路径错误或格式错误)
    ECCS_ERR_CFG_WRITE_FAILED = 201, // 配置文件写入失败
    ECCS_ERR_CFG_KEY_NOT_FOUND = 202, // 属性键名不存在

    // === 授权与系统 (300 - 399) ===
    ECCS_ERR_AUTH_EXPIRED = 300, // 授权过期
    ECCS_ERR_PERMISSION_DENIED = 301  // 无权限
};

// -----------------------------------------------------------
// 设备类型定义
// -----------------------------------------------------------
enum ECCS_DevType {
    ECCS_DEV_UNKNOWN = 0,
    ECCS_DEV_LIGHT = 1, // 强光
    ECCS_DEV_SOUND = 2, // 强声
    ECCS_DEV_PTZ = 3, // 云台
    ECCS_DEV_ULTRASONIC = 4, // 超声
    ECCS_DEV_CAMERA = 5  // 摄像头
};

// -----------------------------------------------------------
// 回调事件类型
// -----------------------------------------------------------
enum ECCS_EventType {
    ECCS_EVT_UNKNOWN = 0,
    ECCS_EVT_STATUS_CHANGE = 1, // 状态变更 (Payload: DeviceStatus结构体)
    ECCS_EVT_PTZ_ANGLE = 2, // 云台角度 (Payload: PtzPosition结构体)
    ECCS_EVT_SOUND_FINISH = 3  // 播放结束 (无 Payload)
};

// -----------------------------------------------------------
// 错误描述
// -----------------------------------------------------------
inline const char* ECCS_GetErrorStr(ECCS_Error code) {
    switch (code) {
    case ECCS_SUCCESS:               return "Success";

        // General
    case ECCS_ERR_FAILED:            return "General Failure";
    case ECCS_ERR_INVALID_PARAM:     return "Invalid Parameter";
    case ECCS_ERR_NOT_INIT:          return "SDK Not Initialized";
    case ECCS_ERR_NOT_SUPPORTED:     return "Operation Not Supported";
    case ECCS_ERR_TIMEOUT:           return "Timeout";
    case ECCS_ERR_MALLOC:            return "Memory Allocation Failed";

        // Device
    case ECCS_ERR_DEV_NOT_FOUND:     return "Device Not Found / Invalid Handle";
    case ECCS_ERR_DEV_OFFLINE:       return "Device Offline";
    case ECCS_ERR_DEV_BUSY:          return "Device Busy";
    case ECCS_ERR_DEV_TYPE_MISMATCH: return "Device Type Mismatch";
    case ECCS_ERR_DEV_SEND_FAILED:   return "Send Data Failed";

        // Config
    case ECCS_ERR_CFG_LOAD_FAILED:   return "Config Load Failed";
    case ECCS_ERR_CFG_WRITE_FAILED:  return "Config Write Failed";
    case ECCS_ERR_CFG_KEY_NOT_FOUND: return "Config Key Not Found";

        // Auth
    case ECCS_ERR_AUTH_EXPIRED:      return "License Expired";
    case ECCS_ERR_PERMISSION_DENIED: return "Permission Denied";

    default:                         return "Unknown Error";
    }
}

#endif // ECHO_CTRL_CODE_H