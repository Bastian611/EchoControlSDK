#pragma once
#include "EchoControlCode.h"

// =======================================================================
// Windows 动态库/静态库 导出控制宏
// =======================================================================
#if defined(_WIN32) || defined(_WIN64)
    #if defined(ECCS_STATIC)
        #define ECCS_API
    #elif defined(ECHOCONTROLSDK_EXPORTS)
        #define ECCS_API __declspec(dllexport)
    #else
        #define ECCS_API __declspec(dllimport)
#endif
#else
    // Linux/Mac
#define ECCS_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

    // =======================================================
    // 基础类型与回调定义
    // =======================================================

    /** 
     * @brief 系统句柄，Init 成功后通过 ECCS_GetHandle 获取 
     */
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

    /**
     * @brief 全双工音频流回调
     * @param data 原始 PCM 数据流 (16kHz PCM)
     */
    typedef void (*ECCSAudioRxCallback)(ECCS_HANDLE hDev, const unsigned char* data, int len, void* userCtx);

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

    /**
     * @brief 注册状态/数据回调
     * @param userCtx 用户自定义指针，回调时原样传回
     */
    ECCS_API ECCS_Error ECCS_RegisterCallback(ECCS_HANDLE hDev, ECCS_CallbackFunc cb, void* userCtx);
    
    // 检查设备是否在线
    ECCS_API bool ECCS_IsOnline(ECCS_HANDLE hDev);

    /** 
     * @brief 手动断开所有设备连接
     * @note 正常状态下不需要调用
     */
    ECCS_API ECCS_Error ECCS_Device_Disconnect(ECCS_HANDLE hDev);

    // =======================================================
    // 强光控制
    // =======================================================

    /** 
     * @brief 强光总开关 
     * @param isOpen，开关: 1=Open, 0=Close
     */
    ECCS_API ECCS_Error ECCS_Light_SetSwitch(ECCS_HANDLE hDev, int isOpen);

    /** 
     * @brief 切换工作模式 
     * @param mode，工作模式(1:不出光, 2:炫目/绿, 3:照明/白) 
     * @note 部分设备固件不支持时，返回ECCS_ERR_NOT_SUPPORTED
     */
    ECCS_API ECCS_Error ECCS_Light_SetMode(ECCS_HANDLE hDev, int mode);

    /** 
     * @brief 设置亮度/功率等级 (1-10) 
     * @param level，亮度: 0-100
     * @note 部分设备固件不支持时，返回ECCS_ERR_NOT_SUPPORTED
     */
    ECCS_API ECCS_Error ECCS_Light_SetLevel(ECCS_HANDLE hDev, int level);

    /** 
     * @brief 设置调焦方式与参数
     * @note 预留接口，当前版本暂不支持此功能
     */
    ECCS_API ECCS_Error ECCS_Light_SetFocus(ECCS_HANDLE hDev, int type, int value);

    /**
     * @brief 开启/关闭频闪
     * @param isOpen，频闪: 1=Open, 0=Close
     * @note 需要在强光总开关开启时调用此方法，否则返回异常
     */
    ECCS_API ECCS_Error ECCS_Light_SetStrobe(ECCS_HANDLE hDev, int isOpen);

    // =======================================================
    // 云台控制
    // =======================================================

    /** 
     * @brief 相对移动控制 
     * @param action，移动: 1=Up, 2=Down, 3=Left, 4=Right, 5=Stop
     *        speed， 速度: 0-64
     */
    ECCS_API ECCS_Error ECCS_PTZ_Move(ECCS_HANDLE hDev, int action, int speed);

    /** 
     * @brief 设置水平线扫角度范围 
     * @param start，线扫起始角度: 0°-359.99°
     *        end，  线扫终止角度: 0°-359.99°
     */
    ECCS_API ECCS_Error ECCS_PTZ_SetScanRange(ECCS_HANDLE hDev, float start, float end);

    /** 
     * @brief 开启自动线扫 
     */
    ECCS_API ECCS_Error ECCS_PTZ_StartScan(ECCS_HANDLE hDev);

    /** 
     * @brief 停止自动线扫 
     */
    ECCS_API ECCS_Error ECCS_PTZ_StopScan(ECCS_HANDLE hDev);

    /** 
     * @brief 云台一键归零/重启 
     */
    ECCS_API ECCS_Error ECCS_PTZ_Reset(ECCS_HANDLE hDev);

    // =======================================================
    // 强声控制
    // =======================================================

    /** 
     * @brief 按索引播放音频文件
     * @param index，音频文件索引号
     *        loop， 是否循环播放（1=循环，0=不循环）
     */
    ECCS_API ECCS_Error ECCS_Sound_Play(ECCS_HANDLE hDev, int index, int loop);

    /** 
     * @brief 停止播放 
     */
    ECCS_API ECCS_Error ECCS_Sound_Stop(ECCS_HANDLE hDev);

    /** 
     * @brief 下一曲 
     */
    ECCS_API ECCS_Error ECCS_Sound_Next(ECCS_HANDLE hDev);

    /** 
     * @brief 上一曲 
     */
    ECCS_API ECCS_Error ECCS_Sound_Prev(ECCS_HANDLE hDev);

    /** 
     * @brief 触发一键驱散音频 
     */
    ECCS_API ECCS_Error ECCS_Sound_OneKeyPlay(ECCS_HANDLE hDev, int index);

    /** 
     * @brief 设置麦克风采集音量
     * @param volume，音量值：0-100（dB）
     */
    ECCS_API ECCS_Error ECCS_Sound_SetCapVolume(ECCS_HANDLE hDev, int volume);

    /** 
     * @brief 设置播放音量
     * @param volume，音量值：0-100（dB）
     */
    ECCS_API ECCS_Error ECCS_Sound_SetPlayVolume(ECCS_HANDLE hDev, int volume);

    /** 
     * @brief 同步获取当前播放音量 
     * @param volume，音量值：0-100（dB）
     */
    ECCS_API ECCS_Error ECCS_Sound_QueryPlayVolume(ECCS_HANDLE hDev, int* volume);

    /** 
     * @brief 同步获取设备内的音频列表 
     */
    ECCS_API ECCS_Error ECCS_Sound_QueryAudioList(ECCS_HANDLE hDev);

    /** @brief 喊话模式开关 */
    ECCS_API ECCS_Error ECCS_Sound_SetMic(ECCS_HANDLE hDev, int isOpen);

    /** @brief 推送原始音频数据流 (全双工上行) */
    ECCS_API ECCS_Error ECCS_Sound_PushData(ECCS_HANDLE hDev, const char* data, int len);

    /** @brief 上传本地音频文件到设备存储 */
    ECCS_API ECCS_Error ECCS_Sound_UploadFile(ECCS_HANDLE hDev, const char* localPath);

    /** @brief 注册全双工音频流回传回调 */
    ECCS_API ECCS_Error ECCS_Sound_RegisterAudioCallback(ECCS_HANDLE hDev, ECCSAudioRxCallback cb, void* userCtx);

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

    // =======================================================
    // 超声控制
    // =======================================================

    /**
     * @brief 超声开关控制
     * @param hSystem 系统句柄
     * @param channel 通道号 (1=通道1, 2=通道2..., 0=所有)
     * @param isOpen  1=开启, 0=关闭
     */
    ECCS_API ECCS_Error ECCS_Ultrasonic_SetSwitch(ECCS_HANDLE hSystem, int channel, int isOpen);

#ifdef __cplusplus
}
#endif