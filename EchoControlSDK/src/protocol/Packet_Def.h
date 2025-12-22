#pragma once
#include "PacketTemplate.hpp"
#include "Packet_ID.h"
#include "../device/DeviceID.h"
#include "../device/DeviceState.h"

ECCS_BEGIN
namespace rpc {

    using namespace did; // 使用 DeviceType 枚举

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
        u32 deviceID;       // DeviceID Value
        u8  slotID;         // Slot Index

        u8  state;          // Value from enum DevState

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

    // ---------------- Sound Data ----------------
    struct SoundPlayCtrl {
        char filename[128]; // 播放文件
        u8 loop;            // 1=循环
    };
    struct SoundTTSCtrl { char text[256]; };   // TTS
    struct SoundVolCtrl { u8 volume; };        // 音量

    // ---------------- PTZ Data ------------------
    struct PtzMotion { u8 action; u8 speed; }; // 上下左右
    struct PtzPreset { u8 action; u8 index; }; // 增删调预置位
    struct PtzPosition { float pan; float tilt; float zoom; }; // 角度信息

    // ---------------- Setting Data --------------
    struct NetConfig { char ip[32]; u16 port; };
    struct DevName { char name[64]; };

#pragma pack(pop)


    // =============================================================
    // Packet 定义 (分类注册)
    // =============================================================

    // #############################################################
    // CONTROL (实时控制)
    // #############################################################

    // --- Light Control ---
    // 开关
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_LIGHT, 1), bool>      RqLightSwitch;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_LIGHT, 1), Result>    RpLightSwitch;
    // 亮度
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_LIGHT, 2), u8>        RqLightLevel;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_LIGHT, 2), Result>    RpLightLevel;
    // 频闪
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_LIGHT, 3), bool>      RqLightStrobe;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_LIGHT, 3), Result>    RpLightStrobe;

    // --- Sound Control ---
    // 播放文件
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 1), SoundPlayCtrl> RqSoundPlay;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 1), Result>        RpSoundPlay;
    // 停止
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 2), NoneData>  RqSoundStop;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 2), Result>    RpSoundStop;
    // TTS
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 3), SoundTTSCtrl>  RqSoundTTS;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 3), Result>        RpSoundTTS;
    // 喊话
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 4), bool>      RqSoundMic;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 4), Result>    RpSoundMic;

    // --- PTZ Control ---
    // 移动
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_PTZ, 1), PtzMotion> RqPtzMove;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_PTZ, 1), Result>    RpPtzMove;
    // 停止
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_PTZ, 2), NoneData>      RqPtzStop;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_PTZ, 2), Result>    RpPtzStop;
    // 预置位
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_PTZ, 3), PtzPreset>     RqPtzPreset;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_PTZ, 3), Result>    RpPtzPreset;


    // #############################################################
    // QUERY (状态查询 - 主动轮询用)
    // #############################################################

    // --- Light Query ---
    typedef Packet<_APP_RQ_QUERY_ID_(DEVICE_LIGHT, 1), NoneData>      RqQueryLightStatus;
    typedef Packet<_APP_RP_QUERY_ID_(DEVICE_LIGHT, 1), LightStatus>   RpQueryLightStatus;

    // --- PTZ Query ---
    typedef Packet<_APP_RQ_QUERY_ID_(DEVICE_PTZ, 1), NoneData>        RqQueryPtzPos;
    typedef Packet<_APP_RP_QUERY_ID_(DEVICE_PTZ, 1), PtzPosition>     RpQueryPtzPos;

    // --- Sound Query ---


    // #############################################################
    // SETTING (参数配置)
    // #############################################################

    // 通用：修改网络配置
    typedef Packet<_APP_RQ_SETTING_ID_(DEVICE_UNKNOWN, 1), NetConfig>   RqSetNetConfig;
    typedef Packet<_APP_RP_SETTING_ID_(DEVICE_UNKNOWN, 1), Result>      RpSetNetConfig;

    // 通用：修改设备名称
    typedef Packet<_APP_RQ_SETTING_ID_(DEVICE_UNKNOWN, 2), DevName>     RqSetDevName;
    typedef Packet<_APP_RP_SETTING_ID_(DEVICE_UNKNOWN, 2), Result>      RpSetDevName;

    // --- Light Setting ---


    // --- Sound Setting ---

    // 音量设置
    typedef Packet<_APP_RQ_SETTING_ID_(DEVICE_SOUND, 1), SoundVolCtrl>  RqSetSoundVolume;
    typedef Packet<_APP_RP_SETTING_ID_(DEVICE_SOUND, 1), Result>        RpSetSoundVolume;

    // --- PTZ Setting ---


    // #############################################################
    // ONEWAY (服务端主动推送)
    // #############################################################

    // 强光状态变更推送
    typedef Packet<_APP_OW_ID_(DEVICE_LIGHT, 1), LightStatus>  OwLightStatus;

    // 强声播放结束推送
    typedef Packet<_APP_OW_ID_(DEVICE_SOUND, 1), NoneData>     OwSoundPlayEnd;

    // 云台位置实时回传
    typedef Packet<_APP_OW_ID_(DEVICE_PTZ, 1), PtzPosition>    OwPtzPosition;

    // 设备状态实时回传
    typedef Packet<_APP_OW_ID_(DEVICE_UNKNOWN, 1), DeviceStatus> OwDeviceStatus;

}
ECCS_END