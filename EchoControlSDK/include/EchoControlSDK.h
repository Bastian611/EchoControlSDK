#pragma once
#include "EchoControlCode.h"

// 动态库导出宏
#ifdef _WIN32
#ifdef ECHOCONTROLSDK_EXPORTS
#define ECCS_API __declspec(dllexport)
#else
#define ECCS_API __declspec(dllimport)
#endif
#else
#define ECCS_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

    // =======================================================
    // 基础类型与回调定义
    // =======================================================

    // 设备句柄
    typedef void* ECCS_HANDLE;
#define ECCS_INVALID_HANDLE NULL

    /**
     * @brief 通用回调函数指针
     * @param hDev 触发事件的设备句柄
     * @param type 事件类型 (ECCS_EventType)
     * @param data 数据指针 (结构体二进制数据)
     * @param len  数据长度
     * @param userCtx 用户注册时传入的上下文指针
     */
    typedef void (*ECCS_CallbackFunc)(ECCS_HANDLE hDev, ECCS_EventType type, const void* data, int len, void* userCtx);

    // =======================================================
    // 系统管理接口
    // =======================================================

    /**
     * @brief 获取 SDK 版本号
     */
    ECCS_API const char* ECCS_GetVersion();

    /**
     * @brief 初始化 SDK     
	 * @return ECCS_RET_SUCCESS 成功
     */
    ECCS_API ECCS_Error ECCS_Init();

    /**
     * @brief 获取系统句柄
     * @note 必须在 Init 成功后调用。
     */
    ECCS_API ECCS_HANDLE ECCS_GetHandle();

    /**
     * @brief 释放 SDK 资源 (停止所有线程，关闭网络)
     */
    ECCS_API void ECCS_Release();

    // =======================================================
    // 通用设备功能
    // =======================================================
/**
     * @brief 注册状态/数据回调
     * @param userCtx 用户自定义指针，回调时原样传回
     */
    ECCS_API ECCS_Error ECCS_RegisterCallback(ECCS_HANDLE hDev, ECCS_CallbackFunc cb, void* userCtx);
    
    // 检查设备是否在线
    ECCS_API bool ECCS_IsOnline(ECCS_HANDLE hDev);

    // =======================================================
    // 强光控制
    // =======================================================

    // 开关: 1=Open, 0=Close
    ECCS_API ECCS_Error ECCS_Light_SetSwitch(ECCS_HANDLE hDev, int isOpen);

    // 亮度: 0-100
    ECCS_API ECCS_Error ECCS_Light_SetLevel(ECCS_HANDLE hDev, int level);

    // 频闪: 1=Open, 0=Close
    ECCS_API ECCS_Error ECCS_Light_SetStrobe(ECCS_HANDLE hDev, int isOpen);

    // =======================================================
    // 云台控制
    // =======================================================

    // 移动: action(1=Up, 2=Down, 3=Left, 4=Right, 5=Stop)
    // speed: 0-64
    ECCS_API ECCS_Error ECCS_PTZ_Move(ECCS_HANDLE hDev, int action, int speed);

    // 变倍: isZoomIn(1=Tele/拉近, 0=Wide/推远)
    ECCS_API ECCS_Error ECCS_PTZ_Zoom(ECCS_HANDLE hDev, int isZoomIn);

    // 预置位: action(1=Set, 2=Goto), index(1-255)
    ECCS_API ECCS_Error ECCS_PTZ_Preset(ECCS_HANDLE hDev, int action, int index);

    // =======================================================
    // 强声控制
    // =======================================================

    // 播放: filename(文件名或索引), loop(1=循环)
    ECCS_API ECCS_Error ECCS_Sound_Play(ECCS_HANDLE hDev, const char* filename, int loop);

    ECCS_API ECCS_Error ECCS_Sound_Stop(ECCS_HANDLE hDev);

    ECCS_API ECCS_Error ECCS_Sound_SetVolume(ECCS_HANDLE hDev, int volume); // 0-100

    ECCS_API ECCS_Error ECCS_Sound_TTS(ECCS_HANDLE hDev, const char* text);

    // 喊话模式: 1=开启, 0=关闭
    ECCS_API ECCS_Error ECCS_Sound_SetMic(ECCS_HANDLE hDev, int isOpen);

    /**
     * @brief 推送音频流数据 (直接写入内部缓冲区)
     * @param hDev 设备句柄
     * @param data 音频数据指针 (PCM/MP3)
     * @param len  数据长度
     * @return ECCS_RET_SUCCESS 成功, ECCS_DEVICE_BUSY 缓冲区满(丢弃)
     */
    ECCS_API ECCS_Error ECCS_Sound_PushData(ECCS_HANDLE hDev, const char* data, int len);

#ifdef __cplusplus
}
#endif