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
        FACTORY_ID_APPEND(RqLightLevel, RpcPacket)
        FACTORY_ID_APPEND(RpLightLevel, RpcPacket)
        FACTORY_ID_APPEND(RqLightStrobe, RpcPacket)
        FACTORY_ID_APPEND(RpLightStrobe, RpcPacket)

        // --- Sound Control ---
        FACTORY_ID_APPEND(RqSoundPlay, RpcPacket)
        FACTORY_ID_APPEND(RpSoundPlay, RpcPacket)
        FACTORY_ID_APPEND(RqSoundStop, RpcPacket)
        FACTORY_ID_APPEND(RpSoundStop, RpcPacket)
        FACTORY_ID_APPEND(RqSoundTTS, RpcPacket)
        FACTORY_ID_APPEND(RpSoundTTS, RpcPacket)
        FACTORY_ID_APPEND(RqSoundMic, RpcPacket)
        FACTORY_ID_APPEND(RpSoundMic, RpcPacket)

        // --- PTZ Control ---
        FACTORY_ID_APPEND(RqPtzMove, RpcPacket)
        FACTORY_ID_APPEND(RpPtzMove, RpcPacket)
        FACTORY_ID_APPEND(RqPtzStop, RpcPacket)
        FACTORY_ID_APPEND(RpPtzStop, RpcPacket)
        FACTORY_ID_APPEND(RqPtzPreset, RpcPacket)
        FACTORY_ID_APPEND(RpPtzPreset, RpcPacket)


        // #############################################################
        // QUERY (状态查询)
        // #############################################################

        // --- Light ---
        FACTORY_ID_APPEND(RqQueryLightStatus, RpcPacket)
        FACTORY_ID_APPEND(RpQueryLightStatus, RpcPacket)

        // --- Sound ---


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


        // --- Sound ---
        FACTORY_ID_APPEND(RqSetSoundVolume, RpcPacket)
        FACTORY_ID_APPEND(RpSetSoundVolume, RpcPacket)
        

        // --- PTZ ---


        // #############################################################
        // ONEWAY (服务端主动推送)
        // #############################################################

        FACTORY_ID_APPEND(OwLightStatus, RpcPacket)
        FACTORY_ID_APPEND(OwSoundPlayEnd, RpcPacket)
        FACTORY_ID_APPEND(OwPtzPosition, RpcPacket)
        FACTORY_ID_APPEND(OwDeviceStatus, RpcPacket)

        FACTORY_END(u32, RpcPacket)

}
ECCS_END