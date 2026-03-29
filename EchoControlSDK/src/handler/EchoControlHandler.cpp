#include "EchoControlHandler.h"
#include "protocol/Packet_Def.h"
#include "debug/Logger.h"

// 引入中间层接口 (Interface Layer)
#include "device/Light/ILight_Device.h"
#include "device/PTZ/IPTZ_Device.h"
#include "device/Sound/ISound_Device.h"
#include "device/Ultrasonic/IUltrasonic_Device.h"

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
    // 强光设备 (Light)
    // =======================================================

    // 开关
    Register<rpc::RqLightSwitch>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ILight_Device, light);
        CAST_PKT(rpc::RqLightSwitch, req);
        light->SetLightSwitch(req->data); 
        });

    // 亮度
    Register<rpc::RqLightWorkMode>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ILight_Device, light);
        CAST_PKT(rpc::RqLightWorkMode, req);
        light->SetWorkMode((RGB_V3_LightMode)req->data); 
        });

    // 频闪
    Register<rpc::RqLightStrobe>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ILight_Device, light);
        CAST_PKT(rpc::RqLightStrobe, req);
        light->SetStrobe(req->data); 
        });

    // Light 调焦 (RGB_V3)
    Register<rpc::RqLightFocus>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ILight_Device, light);
        CAST_PKT(rpc::RqLightFocus, req);
        light->SetFocus((RGB_V3_FocusType)req->data.type, req->data.value);
        });

    // =======================================================
    // 云台设备 (PTZ)
    // =======================================================

    // 移动 (上/下/左/右)
    Register<rpc::RqPtzMove>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(IPTZ_Device, ptz);
        CAST_PKT(rpc::RqPtzMove, req); 
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

    // 归零
    Register<rpc::RqPtzReset>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(IPTZ_Device, ptz);
        ptz->PtzReset();
        });

    // 绝对定位
    Register<rpc::RqPtzAbsolutePos>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(IPTZ_Device, ptz);
        CAST_PKT(rpc::RqPtzAbsolutePos, req);
        ptz->PtzSetAbsolutePos(req->data.pan, req->data.tilt);
        });

    // 开启线扫
    Register<rpc::RqPtzStartScan>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(IPTZ_Device, ptz);
        ptz->PtzStartScan();
        });

    // 停止线扫
    Register<rpc::RqPtzStopScan>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(IPTZ_Device, ptz);
        ptz->PtzStopScan();
        });

    // =======================================================
    // 强声设备 (Sound)
    // =======================================================

    // 播放文件
    Register<rpc::RqSoundPlayFile>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        CAST_PKT(rpc::RqSoundPlayFile, req);
        LOG_WARNING("Sound PlayFile: %s (Not fully implemented in driver)", req->data.filename);
        });

    // 按索引播放
    Register<rpc::RqSoundPlayIndex>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        CAST_PKT(rpc::RqSoundPlayIndex, req);
        sound->PlayIndex(req->data.index, req->data.loop != 0);
        });

    // 停止
    Register<rpc::RqSoundStop>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        sound->StopPlay();
        });

    Register<rpc::RqSoundPrev>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        sound->Prev();
        });

    Register<rpc::RqSoundNext>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        sound->Next();
        });

    // 开启喊话模式
    Register<rpc::RqSoundMicSwitch>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        CAST_PKT(rpc::RqSoundMicSwitch, req);
        sound->MicSwitch(req->data); 
        });

    Register<rpc::RqSoundRTAudio>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        CAST_PKT(rpc::RqSoundRTAudio, req);
        sound->PushAudio(req->data.data, req->data.len);
        });


    // =======================================================
    // 超声设备 (Ultrasonic)
    // =======================================================

    // 开关控制
    Register<rpc::RqUltrasonicSwitch>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(IUltrasonic_Device, ultrasonic);
        CAST_PKT(rpc::RqUltrasonicSwitch, req);

        // req->data.channel: 1, 2, 3...
        // req->data.isOpen: 0, 1
        ultrasonic->SetSwitch(req->data.channel, (req->data.isOpen != 0));
    });

    // =======================================================
    // [CATEGORY] QUERY - 状态查询 
    // =======================================================

    Register<rpc::RqQueryPtzPos>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(IPTZ_Device, ptz);
        ptz->PtzQueryPosition();
        });

    Register<rpc::RqQueryPlayVolume>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        CAST_PKT(rpc::RqQueryPlayVolume, req);
        SoundVolume sv;
        sound->GetPlayVolume(sv);
        });

    Register<rpc::RqQueryAudioList>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        SoundAudioList list;
        sound->GetAudioList(list);
        });

    // =======================================================
    // [CATEGORY] SETTING - 参数配置
    // =======================================================

    Register<rpc::RqSetLightLevel>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ILight_Device, light);
        CAST_PKT(rpc::RqSetLightLevel, req);
        light->SetLightLevel(req->data);
        });

    Register<rpc::RqSetLightFlashFreq>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ILight_Device, light);
        CAST_PKT(rpc::RqSetLightFlashFreq, req);
        light->SetFlashFreq(req->data);
        });

    Register<rpc::RqSetPtzScanRange>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(IPTZ_Device, ptz);
        CAST_PKT(rpc::RqSetPtzScanRange, req);
        ptz->PtzSetScanRange(req->data.start, req->data.end);
        });

    Register<rpc::RqSetSoundPlayVolume>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        CAST_PKT(rpc::RqSetSoundPlayVolume, req);
        sound->SetPlayVolume(req->data);
        });

    Register<rpc::RqSetSoundCapVolume>([](DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt) {
        CAST_DEV(ISound_Device, sound);
        CAST_PKT(rpc::RqSetSoundCapVolume, req);
        sound->SetCaptureVolume(req->data);
        });
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