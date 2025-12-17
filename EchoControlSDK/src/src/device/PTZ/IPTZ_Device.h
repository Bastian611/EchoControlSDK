#pragma once
#include "../DeviceBase.h"

ECCS_BEGIN

class IPTZ_Device : public DeviceBase
{
public:
    virtual void OnPacketReceived(std::shared_ptr<rpc::RpcPacket> pkt) override {
        u32 id = pkt->GetID();

        // 移动 (上/下/左/右/停止)
        if (id == rpc::RqPtzMove::_FACTORY_ID_) {
            auto req = std::dynamic_pointer_cast<rpc::RqPtzMove>(pkt);
            PtzMove(req->data.action, req->data.speed);
            return;
        }

        // 停止 (单独的停止指令，或者复用 Move 的 action=Stop)
        if (id == rpc::RqPtzStop::_FACTORY_ID_) {
            PtzStop();
            return;
        }

        // 预置位
        if (id == rpc::RqPtzPreset::_FACTORY_ID_) {
            auto req = std::dynamic_pointer_cast<rpc::RqPtzPreset>(pkt);
            PtzPreset(req->data.action, req->data.index);
            return;
        }
    }

protected:
    // === 纯虚接口 ===
    // action 定义参考 PacketDef.h (1=Up, 2=Down...)
    virtual void PtzMove(u8 action, u8 speed) = 0;
    virtual void PtzStop() = 0;
    virtual void PtzPreset(u8 action, u8 index) = 0;
};

ECCS_END