#pragma once
#include "PacketTemplate.hpp"
#include "Packet_ID.h"
#include "../device/DeviceID.h"
#include "../device/DeviceState.h"
#include "../device/DeviceDataTypes.h"

ECCS_BEGIN
namespace rpc {

    using namespace did; // 使用 DeviceType 枚举


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
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_LIGHT, 2), u8>        RqLightWorkMode;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_LIGHT, 2), Result>    RpLightWorkMode;
    // 频闪
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_LIGHT, 3), bool>      RqLightStrobe;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_LIGHT, 3), Result>    RpLightStrobe;

    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_LIGHT, 4), LightFocus>    RqLightFocus;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_LIGHT, 4), Result>        RpLightFocus;

    // --- Sound Control ---
    // 播放文件
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 1), SoundPlayFile> RqSoundPlayFile;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 1), Result>        RpSoundPlayFile;
    // 按索引播放
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 2), SoundPlayIndex> RqSoundPlayIndex;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 2), Result>         RpSoundPlayIndex;
    // 停止
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 3), NoneData>  RqSoundStop;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 3), Result>    RpSoundStop;
    // 上一曲
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 4), NoneData>  RqSoundPrev;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 4), Result>    RpSoundPrev;
    // 下一曲
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 5), NoneData>  RqSoundNext;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 5), Result>    RpSoundNext;
    // 一键驱散
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 6), int>       RqSoundOneKey;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 6), Result>    RpSoundOneKey;
    // 喊话
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 7), bool>      RqSoundMicSwitch;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 7), Result>    RpSoundMicSwitch;

    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_SOUND, 8), SoundAudioBuffer>  RqSoundRTAudio;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_SOUND, 8), Result>            RpSoundRTAudio;

    // --- PTZ Control ---
    // 移动
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_PTZ, 1), PtzMotion> RqPtzMove;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_PTZ, 1), Result>    RpPtzMove;
    // 停止
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_PTZ, 2), NoneData> RqPtzStop;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_PTZ, 2), Result>   RpPtzStop;
    // 预置位
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_PTZ, 3), PtzPreset> RqPtzPreset;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_PTZ, 3), Result>    RpPtzPreset;

    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_PTZ, 4), NoneData>    RqPtzReset;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_PTZ, 4), Result>      RpPtzReset;

    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_PTZ, 5), PtzPosition> RqPtzAbsolutePos;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_PTZ, 5), Result>      RpPtzAbsolutePos;

    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_PTZ, 6), NoneData> RqPtzStartScan;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_PTZ, 6), Result>   RpPtzStartScan;

    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_PTZ, 7), NoneData> RqPtzStopScan;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_PTZ, 7), Result>   RpPtzStopScan;

    // 继电器控制
    typedef Packet<_APP_RQ_CONTROL_ID_(DEVICE_ULTRASONIC, 1), UltrasonicSwitch>  RqUltrasonicSwitch;
    typedef Packet<_APP_RP_CONTROL_ID_(DEVICE_ULTRASONIC, 1), Result>            RpUltrasonicSwitch;


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
    typedef Packet<_APP_RQ_QUERY_ID_(DEVICE_SOUND, 1), NoneData>        RqQueryAudioList;
    typedef Packet<_APP_RP_QUERY_ID_(DEVICE_SOUND, 1), SoundAudioList>  RpQueryAudioList;

    typedef Packet<_APP_RQ_QUERY_ID_(DEVICE_SOUND, 2), NoneData>    RqQueryCapVolume;
    typedef Packet<_APP_RP_QUERY_ID_(DEVICE_SOUND, 2), u8>          RpQueryCapVolume;

    typedef Packet<_APP_RQ_QUERY_ID_(DEVICE_SOUND, 3), NoneData>    RqQueryPlayVolume;
    typedef Packet<_APP_RP_QUERY_ID_(DEVICE_SOUND, 3), SoundVolume> RpQueryPlayVolume;

    typedef Packet<_APP_RQ_QUERY_ID_(DEVICE_SOUND, 4), NoneData>        RqQuerySoundStatus;
    typedef Packet<_APP_RP_QUERY_ID_(DEVICE_SOUND, 4), SoundStatusData> RpQuerySoundStatus;

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

    // 功率档位
    typedef Packet<_APP_RQ_SETTING_ID_(DEVICE_LIGHT, 1), u8>      RqSetLightLevel; 
    typedef Packet<_APP_RP_SETTING_ID_(DEVICE_LIGHT, 1), Result>  RpSetLightLevel;
    // 频闪频率
    typedef Packet<_APP_RQ_SETTING_ID_(DEVICE_LIGHT, 2), u8>      RqSetLightFlashFreq;  
    typedef Packet<_APP_RP_SETTING_ID_(DEVICE_LIGHT, 2), Result>  RpSetLightFlashFreq;

    // --- Sound Setting ---

    // 音量设置
    typedef Packet<_APP_RQ_SETTING_ID_(DEVICE_SOUND, 1), u8>        RqSetSoundCapVolume;
    typedef Packet<_APP_RP_SETTING_ID_(DEVICE_SOUND, 1), Result>    RpSetSoundCapVolume;

    typedef Packet<_APP_RQ_SETTING_ID_(DEVICE_SOUND, 2), u8>        RqSetSoundPlayVolume;
    typedef Packet<_APP_RP_SETTING_ID_(DEVICE_SOUND, 2), Result>    RpSetSoundPlayVolume;

    // --- PTZ Setting ---
    typedef Packet<_APP_RQ_SETTING_ID_(DEVICE_PTZ, 1), PtzScanRange>    RqSetPtzScanRange;
    typedef Packet<_APP_RP_SETTING_ID_(DEVICE_PTZ, 1), Result>          RpSetPtzScanRange;

    // #############################################################
    // ONEWAY (主动推送)
    // #############################################################

    // 通用健康状态推送 (心跳或故障时触发)
    typedef Packet<_APP_OW_ID_(DEVICE_UNKNOWN, 1), DeviceStatus> OwDeviceStatus;

    // 云台位置实时回传 (FF 09 指令触发后的持续推送)
    typedef Packet<_APP_OW_ID_(DEVICE_PTZ, 1), PtzPosition> OwPtzPosition;

    // 强声播放结束/切换通知
    typedef Packet<_APP_OW_ID_(DEVICE_SOUND, 1), SoundStatusData> OwSoundStatus;

    // 强光实时参数反馈 (RGB_V3 状态回传)
    typedef Packet<_APP_OW_ID_(DEVICE_LIGHT, 1), LightRTParam>   OwLightStatus;

}
ECCS_END