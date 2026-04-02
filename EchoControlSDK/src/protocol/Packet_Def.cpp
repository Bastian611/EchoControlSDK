#include "Packet_Def.h"

ECCS_BEGIN
namespace rpc {

    // -------------------------------------------------------------
    // 工厂注册入口
    // -------------------------------------------------------------
    // RpcPacket 是基类，u32 是 Key (Packet ID)
    // 这里的注册顺序不影响逻辑，但建议按分类保持整洁

    FACTORY_BEGIN(u32, RpcPacket)

    // #############################################################
    // CONTROL (实时控制)
    // #############################################################

    // --- Light Control ---
    FACTORY_ID_APPEND(RqLightSwitch, RpcPacket)
    FACTORY_ID_APPEND(RpLightSwitch, RpcPacket)
    FACTORY_ID_APPEND(RqLightWorkMode, RpcPacket)
    FACTORY_ID_APPEND(RpLightWorkMode, RpcPacket)
    FACTORY_ID_APPEND(RqLightStrobe, RpcPacket)
    FACTORY_ID_APPEND(RpLightStrobe, RpcPacket)
    FACTORY_ID_APPEND(RqLightFocus, RpcPacket)
    FACTORY_ID_APPEND(RpLightFocus, RpcPacket)

    // --- Sound Control ---
    FACTORY_ID_APPEND(RqSoundPlayFile, RpcPacket)
    FACTORY_ID_APPEND(RpSoundPlayFile, RpcPacket)
    FACTORY_ID_APPEND(RqSoundPlayIndex, RpcPacket)
    FACTORY_ID_APPEND(RpSoundPlayIndex, RpcPacket)
    FACTORY_ID_APPEND(RqSoundStop, RpcPacket)
    FACTORY_ID_APPEND(RpSoundStop, RpcPacket)
    FACTORY_ID_APPEND(RqSoundPrev, RpcPacket)
    FACTORY_ID_APPEND(RpSoundPrev, RpcPacket)
    FACTORY_ID_APPEND(RqSoundNext, RpcPacket)
    FACTORY_ID_APPEND(RpSoundNext, RpcPacket)
    FACTORY_ID_APPEND(RqSoundOneKey, RpcPacket)
    FACTORY_ID_APPEND(RpSoundOneKey, RpcPacket)
    FACTORY_ID_APPEND(RqSoundMicSwitch, RpcPacket)
    FACTORY_ID_APPEND(RpSoundMicSwitch, RpcPacket)
    FACTORY_ID_APPEND(RqSoundRTAudio, RpcPacket)
    FACTORY_ID_APPEND(RpSoundRTAudio, RpcPacket)
    FACTORY_ID_APPEND(RqSoundSetTalk, RpcPacket)
    FACTORY_ID_APPEND(RpSoundSetTalk, RpcPacket)

    // --- PTZ Control ---
    FACTORY_ID_APPEND(RqPtzMove, RpcPacket)
    FACTORY_ID_APPEND(RpPtzMove, RpcPacket)
    FACTORY_ID_APPEND(RqPtzStop, RpcPacket)
    FACTORY_ID_APPEND(RpPtzStop, RpcPacket)
    FACTORY_ID_APPEND(RqPtzPreset, RpcPacket)
    FACTORY_ID_APPEND(RpPtzPreset, RpcPacket)
    FACTORY_ID_APPEND(RqPtzAbsolutePos, RpcPacket)
    FACTORY_ID_APPEND(RpPtzAbsolutePos, RpcPacket)
    FACTORY_ID_APPEND(RqPtzReset, RpcPacket)
    FACTORY_ID_APPEND(RpPtzReset, RpcPacket)
    FACTORY_ID_APPEND(RqPtzStartScan, RpcPacket)
    FACTORY_ID_APPEND(RpPtzStartScan, RpcPacket)
    FACTORY_ID_APPEND(RqPtzStopScan, RpcPacket)
    FACTORY_ID_APPEND(RpPtzStopScan, RpcPacket)

    // --- Ultrasonic Control ---
    FACTORY_ID_APPEND(RqUltrasonicSwitch, RpcPacket)
    FACTORY_ID_APPEND(RpUltrasonicSwitch, RpcPacket)


    // #############################################################
    // QUERY (状态查询)
    // #############################################################

    // --- Light ---
    FACTORY_ID_APPEND(RqQueryLightStatus, RpcPacket)
    FACTORY_ID_APPEND(RpQueryLightStatus, RpcPacket)

    // --- Sound ---
    FACTORY_ID_APPEND(RqQueryAudioList, RpcPacket)
    FACTORY_ID_APPEND(RpQueryAudioList, RpcPacket)
    FACTORY_ID_APPEND(RqQueryCapVolume, RpcPacket)
    FACTORY_ID_APPEND(RpQueryCapVolume, RpcPacket)
    FACTORY_ID_APPEND(RqQueryPlayVolume, RpcPacket)
    FACTORY_ID_APPEND(RpQueryPlayVolume, RpcPacket)
    FACTORY_ID_APPEND(RqQuerySoundStatus, RpcPacket)
    FACTORY_ID_APPEND(RpQuerySoundStatus, RpcPacket)

    // --- PTZ ---
    FACTORY_ID_APPEND(RqQueryPtzPos, RpcPacket)
    FACTORY_ID_APPEND(RpQueryPtzPos, RpcPacket)


    // #############################################################
    // SETTING (参数配置)
    // #############################################################
    
    // 通用网络配置
    FACTORY_ID_APPEND(RqSetNetConfig, RpcPacket)
    FACTORY_ID_APPEND(RpSetNetConfig, RpcPacket)

    // 通用设备名称
    FACTORY_ID_APPEND(RqSetDevName, RpcPacket)
    FACTORY_ID_APPEND(RpSetDevName, RpcPacket)

    // --- Light ---
    FACTORY_ID_APPEND(RqSetLightLevel, RpcPacket)
    FACTORY_ID_APPEND(RpSetLightLevel, RpcPacket)
    FACTORY_ID_APPEND(RqSetLightFlashFreq, RpcPacket)
    FACTORY_ID_APPEND(RpSetLightFlashFreq, RpcPacket)

    // --- Sound ---
    FACTORY_ID_APPEND(RqSetSoundCapVolume, RpcPacket)
    FACTORY_ID_APPEND(RpSetSoundCapVolume, RpcPacket)
    FACTORY_ID_APPEND(RqSetSoundPlayVolume, RpcPacket)
    FACTORY_ID_APPEND(RpSetSoundPlayVolume, RpcPacket)
        

    // --- PTZ ---
    FACTORY_ID_APPEND(RqSetPtzScanRange, RpcPacket)
    FACTORY_ID_APPEND(RpSetPtzScanRange, RpcPacket)

    // #############################################################
    // ONEWAY (服务端主动推送)
    // #############################################################

    FACTORY_ID_APPEND(OwLightStatus, RpcPacket)
    FACTORY_ID_APPEND(OwSoundStatus, RpcPacket)
    FACTORY_ID_APPEND(OwPtzPosition, RpcPacket)
    FACTORY_ID_APPEND(OwDeviceStatus, RpcPacket)

    FACTORY_END(u32, RpcPacket)

}
ECCS_END