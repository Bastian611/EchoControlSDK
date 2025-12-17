#pragma once
#include "../Device.hpp"

ECCS_BEGIN

class ILight_Device : public Device
{
public:
    // 覆盖基类的 Packet 处理函数
    virtual void OnPacketReceived(std::shared_ptr<rpc::RpcPacket> pkt) override {
        u32 id = pkt->GetID();

        // 开关
        if (id == rpc::RqLightSwitch::_FACTORY_ID_) {
            auto req = std::dynamic_pointer_cast<rpc::RqLightSwitch>(pkt);
            SetSwitch(req->data); // data 是 bool
            return;
        }

        // 亮度
        if (id == rpc::RqLightLevel::_FACTORY_ID_) {
            auto req = std::dynamic_pointer_cast<rpc::RqLightLevel>(pkt);
            SetBrightness(req->data); // data 是 u8
            return;
        }

        // 频闪
        if (id == rpc::RqLightStrobe::_FACTORY_ID_) {
            auto req = std::dynamic_pointer_cast<rpc::RqLightStrobe>(pkt);
            SetStrobe(req->data); // data 是 bool
            return;
        }

        // 状态查询 (通用处理，子类需维护状态缓存)
        if (id == rpc::RqQueryLightStatus::_FACTORY_ID_) {
            // 这里可以调用 GetStatus 并回复，暂时留空或由子类处理
            // ...
            return;
        }
    }

    // --- 兼容旧字符串指令 (可选) ---
    virtual void OnRegisterCommands() override {
        RegisterCmd(DeviceEventID::ExecuteString, [this](const str& args) {
            // 解析字符串 args 并调用 SetSwitch 等
            // 建议逐步废弃，这里仅作示例
            });
    }

protected:
    // === 纯虚接口：由具体驱动实现 ===
    virtual void SetSwitch(bool isOpen) = 0;
    virtual void SetBrightness(u8 level) = 0;
    virtual void SetStrobe(bool isOpen) = 0;
};

ECCS_END