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
    // 公共 C 结构体定义
    // =======================================================

#pragma pack(push, 1)

    /** 
     * @brief 强光状态信息 
     * @param isOpen，开关状态
     * @param brightness，亮度
     * @param strobeFreq，频闪频率
     * @param tempture，温度
     * @note 当前版本未启用
     */
    typedef struct {
        unsigned char isOpen;
        unsigned char brightness;
        unsigned char strobeFreq;
        float temperature;
    } ECCS_LightStatus;

    /** 
     * @brief 云台位置信息
     * @param pan，水平角度信息
     * @param tilt，垂直角度信息
     * @param zoom，变倍（预留）
     */
    typedef struct {
        float pan;
        float tilt;
        float zoom;
    } ECCS_PtzPosition;

    /**
     * @brief 强声模块状态
     * @param mode，播放模式，0：空闲，1：播放器模式，2：一键驱鸟音播放模式，3：远程喊话模式
     * @param capVol，设备采集音量
     * @param playVol，设备播放音量
     * @param curIndex，当前播放的文件索引，-1代表未进行播放或未查询到
     */
    typedef struct {
        uint8_t     mode;
        uint8_t     capVol;
        uint8_t     playVol;
        uint16_t    curIndex;
    } ECCS_SoundStatusData;

    /** 
     * @brief 音频文件简要信息
     * @param index，文件索引号
     * @param name，文件名称
     */
    typedef struct {
        int index;
        char name[64];
    } ECCS_AudioFileInfo;

    /** 
     * @brief 音频文件列表 
     * @param count，音频文件数量
     * @param files，音频文件简要信息，目前最大支持200个音频文件
     */
    typedef struct {
        unsigned short count;
        ECCS_AudioFileInfo files[200];
    } ECCS_SoundAudioList;

    /**
     * @brief 一键拒止业务参数集
     * 涵盖声、光、电三个维度的预设执行参数
     */
    typedef struct {
        // =================================================
        // 强声参数
        // =================================================
        unsigned char soundVolume;      // 播放音量: 0-100
        int           soundTrackIndex;  // 预设音频索引 (需提前刷新列表确认索引存在)
        unsigned char soundLoop;        // 循环模式: 1=循环播放, 0=单次播放

        // =================================================
        // 强光参数
        // =================================================
        unsigned char lightMode;        // 工作模式: 1:不出光, 2:炫目/绿, 3:照明/白（部分型号不支持）
        unsigned char lightLevel;       // 功率/亮度档位: 1-10（部分型号不支持）
        unsigned char lightStrobe;      // 频闪控制: 1=开启, 0=关闭

        // =================================================
        // 云台参数
        // =================================================
        float         ptzScanStart;     // 水平线扫起始角度: 0.00°- 359.99°
        float         ptzScanEnd;       // 水平线扫终止角度: 0.00°- 359.99°
    } ECCS_OneKeyParams;

    typedef struct {
        uint8_t     deviceType;
        uint8_t     deviceIndex;
        uint8_t     state;           // Value from enum DevState
        uint32_t    errorCode;      // 错误码
        float       temperature;  // 温度
    } ECCS_DeviceStatus;

#pragma pack(pop)

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
     * @param len  数据长度
     * @param userCtx 用户注册时传入的上下文指针
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
    
    /** 
     * @brief 检查设备是否在线
     * @note 
     */
    ECCS_API bool ECCS_IsOnline(ECCS_HANDLE hDev);

    /** 
     * @brief 手动断开所有设备连接
     * @note 正常状态下不需要调用
     */
    ECCS_API ECCS_Error ECCS_Device_Disconnect(ECCS_HANDLE hDev);

    // =======================================================
    // 系统级控制
    // =======================================================

    /** @brief 启动一键拒止 (宏动作：设置参数->线扫->强光->播放) */
    ECCS_API ECCS_Error ECCS_OneKey_Start(ECCS_HANDLE hDev);

    /** @brief 停止一键拒止 (宏动作：关闭声、光、电) */
    ECCS_API ECCS_Error ECCS_OneKey_Stop(ECCS_HANDLE hDev);

    /**
     * @brief 设置一键拒止参数
     * @param params，一键拒止参数
     */
    ECCS_API ECCS_Error ECCS_OneKey_SetParams(ECCS_HANDLE hDev, const ECCS_OneKeyParams* params);
    /**
     * @brief 读取一键拒止参数
     * @param params，一键拒止参数
     */
    ECCS_API ECCS_Error ECCS_OneKey_GetParams(ECCS_HANDLE hDev, ECCS_OneKeyParams* params);

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
     * @param speed， 速度: 0-64
     */
    ECCS_API ECCS_Error ECCS_PTZ_Move(ECCS_HANDLE hDev, int action, int speed);

    /**
     * @brief 控制云台旋转到指定角度
     * @param pan，  水平角度: -45°- +45°
     * @param tilt， 俯仰角度: 0°- 359.99°
     */
    ECCS_API ECCS_Error ECCS_PTZ_SetAbsolutePos(ECCS_HANDLE hDev, float pan, float tilt);

    /** 
     * @brief 设置水平线扫角度范围 
     * @param start，线扫起始角度: 0°-359.99°
     * @param end，  线扫终止角度: 0°-359.99°
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
     * @param loop， 是否循环播放（1=循环，0=不循环）
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
     * @param index，指定索引号的音频，可为0
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
     * @brief 获取当前播放音量 
     * @param playVolume，音量值：0-100（dB）
     * @param capVolume，音量值：0-100（dB）
     */
    ECCS_API ECCS_Error ECCS_Sound_QueryPlayVolume(ECCS_HANDLE hDev, int* playVolume, int* capVolume);

    /** 
     * @brief 获取设备内的音频列表 
     */
    ECCS_API ECCS_Error ECCS_Sound_QueryAudioList(ECCS_HANDLE hDev, ECCS_SoundAudioList* list);

    /** 
     * @brief 喊话模式开关 
     * @param isOpen，开关：1=Open, 0=Close
     */
    ECCS_API ECCS_Error ECCS_Sound_SetMic(ECCS_HANDLE hDev, int isOpen);

    /**
     * @brief 开启/关闭对讲模式 (全双工)
     * @param isOpen 1=开启双向对讲, 0=关闭
     * @note 开启后，SDK 会同时触发音频回传回调
     */
    ECCS_API ECCS_Error ECCS_Sound_SetTalk(ECCS_HANDLE hDev, int isOpen);

    /** 
     * @brief 上传本地音频文件到设备存储 
     * @param localPath，本地音频文件名称，目前只支持mp3/pcm格式文件
     */
    ECCS_API ECCS_Error ECCS_Sound_UploadFile(ECCS_HANDLE hDev, const char* localPath);

    /** 
     * @brief 注册全双工音频流回传回调
     */
    ECCS_API ECCS_Error ECCS_Sound_RegisterAudioCallback(ECCS_HANDLE hDev, ECCSAudioRxCallback cb, void* userCtx);

    /**
     * @brief 推送音频流数据(全双工上行)
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
     * @note 当前版本未启用
     */
    ECCS_API ECCS_Error ECCS_Ultrasonic_SetSwitch(ECCS_HANDLE hSystem, int channel, int isOpen);

#ifdef __cplusplus
}
#endif