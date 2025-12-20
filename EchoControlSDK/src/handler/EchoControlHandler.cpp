#include "EchoControlHandler.h"
#include "protocol/Packet_Def.h"
#include "debug/Logger.h"

// 引入中间层接口 (Interface Layer)
#include "device/Light/ILight_Device.h"
#include "device/PTZ/IPTZ_Device.h"
#include "device/Sound/ISound_Device.h"

ECCS_BEGIN

// -----------------------------------------------------------
// 辅助宏：简化类型转换与安全检查
// -----------------------------------------------------------

// 将 DeviceBase* 转换为具体的中间层接口指针 (如 ILight_Device*)
#define CAST_DEV(InterfaceType, VarName) \
    InterfaceType* VarName = dynamic_cast<InterfaceType*>(dev); \
    if (!VarName) { \
        LOG_ERROR("[EchoHandler] Device type mismatch. Expected: %s", #InterfaceType); \
        return; \
    }

// 将 RpcPacket* 转换为具体的业务包指针 (如 RqLightSwitch*)
#define CAST_PKT(PacketType, VarName) \
    auto VarName = std::dynamic_pointer_cast<PacketType>(pkt); \
    if (!VarName) { \
        LOG_ERROR("[EchoHandler] Packet cast failed. Expected: %s", #PacketType); \
        return; \
    }

// -----------------------------------------------------------
// 构造函数：注册路由表
// -----------------------------------------------------------
EchoControlHandler::EchoControlHandler() {

    // =======================================================
    // 1. 强光设备 (Light)
    // =======================================================

    // 开关
    Register<rpc::RqLightSwitch>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ILight_Device, light);
        CAST_PKT(rpc::RqLightSwitch, req);
        light->SetSwitch(req->data); // bool
        });

    // 亮度
    Register<rpc::RqLightLevel>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ILight_Device, light);
        CAST_PKT(rpc::RqLightLevel, req);
        light->SetBrightness(req->data); // u8
        });

    // 频闪
    Register<rpc::RqLightStrobe>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ILight_Device, light);
        CAST_PKT(rpc::RqLightStrobe, req);
        light->SetStrobe(req->data); // bool
        });

    // =======================================================
    // 2. 云台设备 (PTZ)
    // =======================================================

    // 移动 (上/下/左/右)
    Register<rpc::RqPtzMove>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(IPTZ_Device, ptz);
        CAST_PKT(rpc::RqPtzMove, req); // 注意: 你的 PacketDef 中 PtzMotion 结构体
        ptz->PtzMove(req->data.action, req->data.speed);
        });

    // 停止
    Register<rpc::RqPtzStop>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(IPTZ_Device, ptz);
        ptz->PtzStop();
        });

    // 预置位
    Register<rpc::RqPtzPreset>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(IPTZ_Device, ptz);
        CAST_PKT(rpc::RqPtzPreset, req);
        ptz->PtzPreset(req->data.action, req->data.index);
        });

    // =======================================================
    // 3. 强声设备 (Sound)
    // =======================================================

    // 播放文件
    Register<rpc::RqSoundPlay>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        CAST_PKT(rpc::RqSoundPlay, req);
        sound->PlayFile(req->data.filename, req->data.loop > 0);
        });

    // 停止
    Register<rpc::RqSoundStop>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        sound->StopPlay();
        });

    // TTS
    Register<rpc::RqSoundTTS>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        CAST_PKT(rpc::RqSoundTTS, req);
        sound->TTSPlay(req->data.text);
        });

    // 喊话
    Register<rpc::RqSoundMic>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        CAST_PKT(rpc::RqSoundMic, req);
        sound->SetMic(req->data); // bool
        });

    // 设置音量 (假设 PacketDef.h 中有定义 RqSoundVol，若没定义这里需注释掉或补全)
    /*
    Register<rpc::RqSoundVol>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        CAST_PKT(rpc::RqSoundVol, req);
        sound->SetVolume(req->data.volume);
    });
    */
}

// -----------------------------------------------------------
// 核心分发逻辑
// -----------------------------------------------------------
void EchoControlHandler::Dispatch(DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
    if (!dev || !pkt) return;

    u32 id = pkt->GetID();
    auto it = m_handlers.find(id);

    if (it != m_handlers.end()) {
        // 找到处理函数 -> 执行
        try {
            it->second(dev, pkt);
        }
        catch (std::exception& e) {
            LOG_ERROR("[EchoHandler] Exception during dispatch ID 0x%X: %s", id, e.what());
        }
    }
    else {
        // 未注册的指令
        LOG_WARNING("[EchoHandler] No handler found for Packet ID 0x%X", id);
    }
}

ECCS_END