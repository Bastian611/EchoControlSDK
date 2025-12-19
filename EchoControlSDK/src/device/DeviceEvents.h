#pragma once
#include "../global.h"
#include "../thread/event_queue.h"
#include "../protocol/RpcPacket.h"
#include <string>

ECCS_BEGIN

// -----------------------------------------------------------
// 事件 ID 定义
// -----------------------------------------------------------
namespace DeviceEventID {
    // 避免与系统事件 (EventTypes::Quit = 0) 冲突
    // EventTypes::User 通常是 512

    // 强类型协议包到达事件
    const int PacketArrival = EventTypes::User + 1;

    // 配置更新事件 (例如 IP 变更)
    const int ConfigUpdate = EventTypes::User + 2;
}


// -----------------------------------------------------------
// 协议包事件 (核心)
// -----------------------------------------------------------
class PacketEvent : public Event {
public:
    // 接收智能指针，生命周期自动管理
    PacketEvent(std::shared_ptr<rpc::RpcPacket> pkt)
        : Event(DeviceEventID::PacketArrival), m_packet(pkt) {}

    std::shared_ptr<rpc::RpcPacket> GetPacket() const { return m_packet; }

private:
    std::shared_ptr<rpc::RpcPacket> m_packet;
};


// -----------------------------------------------------------
// 配置更新事件 (可选)
// -----------------------------------------------------------
struct ConfigUpdateData {
    str key;
    str value;
};
typedef EventTemplateEx<DeviceEventID::ConfigUpdate, ConfigUpdateData> ConfigUpdateEvent;

ECCS_END