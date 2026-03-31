#pragma once
#include "../global.h"


ECCS_BEGIN

// =======================
// 播放模式
// =======================
enum SoundStatus
{
    Idle,           // 空闲 / 停止
    Player,         // 播放器（MP3 列表）
    OneKey,         // 一键驱散
    MicBroadcast    // 实时喊话
};

struct SoundStatusData
{
    u8 mode;
    u8 capVol;
    u8 playVol;
};

// =======================
// 播放状态事件
// =======================
enum SoundPlayState
{
    Stopped,
    Playing,
    Finished,
    Error
};

// =======================
// 音频文件信息
// =======================
struct SoundFileInfo
{
    int index;
    str name;
    u32 duration;   // 秒
};

// 音频发送参数控制
struct AudioSpec {
    size_t chunkSize; // 每一包发送的大小
    int intervalMs;   // 网线直连下建议设为很小（如 1-5ms）或 0
};

// RGB V3工作模式
enum class RGB_V3_LightMode : u8 {
    Off = 1,        // 不出光
    Blingding = 2,  // 炫目模式
    Lighting = 3,   // 照明模式
    QueryStatus = 8 // 设备状态查询
};

// 调焦参数
enum class RGB_V3_FocusType : u8 {
    Distance = 1,
    Angle = 2,
    StepAdd = 3,
    StepSub = 4,
    AutoCont = 5,
    AutoSingle = 6,
    Manual = 7
};

/** @brief 视频输出模式内部定义 */
enum class VideoOutMode : u8 {
    RawData = 0,    // 原始码流 (H.264 / H.265)
    YUV420P = 1,    // 解码后的 YUV420 格式 (需调用播放库)
    RGB24 = 2     // 转换后的 RGB 格式 (需调用播放库 + 色彩转换)
};

/** @brief 摄像头调焦动作 */
enum class CameraFocusAction : u8 {
    FocusIn = 1,
    FocusOut = 0
};

// =============================================================
// 通用数据结构 (1字节对齐)
// =============================================================
#pragma pack(push, 1)

    // 通用结果返回
struct Result {
    u32 code;     // 0=Success
    char msg[64];
};

// 通用设备状态数据
struct DeviceStatus {
    u8 deviceType;
    u8 deviceIndex;
    u8 state;           // Value from enum DevState
    u32 errorCode;      // 错误码
    float temperature;  // 温度
};

// ---------------- Light Data ----------------
struct LightStatus {
    u8 isOpen;
    u8 brightness;
    u8 strobeFreq;
    float temperature;
};

struct LightFocus {
    u8 type;    // FocusType 枚举值
    u16 value;  // 距离或角度参数
};

// 强光实时参数 (针对 RGB_V3 的 18 字节回传)
struct LightRTParam {
    u8  mode;           // 当前工作模式 (1,2,3)
    u8  power;          // 当前功率档位
    float actualAngle;  // 调焦后的实际角度 (10倍换算后)
    u16 motorSteps;     // 电机实际步数
};


// ---------------- Sound Data ----------------
struct SoundPlayFile {
    char filename[128]; // 播放文件
    u8 loop;            // 1=循环
};

struct SoundPlayIndex {
    int index;
    u8 loop;
};

struct SoundVolume {
    u8 playVol;
    u8 capVol;
};

struct SoundAudioList {
    u16 count;
    struct {
        int index;
        char name[64]; // 文件名简写
    } files[200];
};

struct SoundAudioBuffer {
    const u8* data;
    u32 len;
};

// 强声播放进度/状态
struct SoundPlayStatusData {
    int currentIndex;   // 当前播放的索引
    u8  playState;      // 对应 SoundPlayState 枚举: Playing, Finished, Error
};

// ---------------- PTZ Data ------------------
struct PtzMotion {
    u8 action;
    u8 speed;
}; // 上下左右

struct PtzPreset {
    u8 action;
    u8 index;
}; // 增删调预置位

struct PtzPosition {
    float pan;
    float tilt;
    float zoom;
}; // 角度信息

struct PtzScanRange {
    float start;
    float end;
};


// ---------------- Setting Data --------------
struct NetConfig {
    char ip[32];
    u16 port;
};

struct DevName {
    char name[64];
};

// ---------------- Ultrasonic Data ----------------
struct UltrasonicSwitch {
    u8 channel; // 0=All, 1=Channel1...
    u8 isOpen;
};

/** @brief 摄像机打包状态 (Query/OW) */
struct CameraStatus {
    u8 streamState;      // 0:未拉流, 1:正常, 2:丢包
    u8 loginState;       // 0:未登录, 1:已登录
    float temperature;   // 设备内部温度
    u8 sdCardState;      // 存储卡状态: 0:无卡, 1:正常, 2:异常
    u32 upTime;          // 运行时间(秒)
};

/** @brief AI 告警数据 (OW) */
struct CameraAlarm {
    u32 alarmType;       // 14:移动侦测, 15:遮挡报警, 16:越界侦测等
    u16 targetX;         // 目标中心点坐标 X (0-10000 归一化)
    u16 targetY;         // 目标中心点坐标 Y
    u16 targetW;         // 目标宽度
    u16 targetH;         // 目标高度
    char timestamp[32];  // 告警时间戳
};

/** @brief 视频流参数 (主动查询结果) */
struct VideoParams {
    u16 width;
    u16 height;
    u8  framerate;
    u32 bitrate;         // Kbps
    u8  encodeFormat;    // 0:H264, 1:H265
};

#pragma pack(pop)

ECCS_END