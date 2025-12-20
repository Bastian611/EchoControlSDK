#pragma once
#include "../global.h"
#include "protocol/RpcPacket.h"
#include <memory>
#include <functional>
#include <map>

ECCS_BEGIN

// 前置声明，避免循环引用头文件
class DeviceBase;

class EchoControlHandler {
public:
    // 单例访问
    static EchoControlHandler& Instance() {
        static EchoControlHandler instance;
        return instance;
    }

    /**
     * @brief 核心分发函数
     * @param dev 目标设备指针 (DeviceBase*)
     * @param pkt 收到的协议包
     */
    void Dispatch(DeviceBase* dev, std::shared_ptr<rpc::RpcPacket> pkt);

private:
    EchoControlHandler(); // 在构造函数中完成所有指令的注册

    // 定义处理函数的签名
    using HandlerFunc = std::function<void(DeviceBase*, std::shared_ptr<rpc::RpcPacket>)>;

    // 模板辅助函数：利用 Packet 自身的 ID 进行注册
    template<typename TPacket>
    void Register(HandlerFunc func) {
        // TPacket::_FACTORY_ID_ 是 PacketTemplate 自动生成的静态常量
        m_handlers[TPacket::_FACTORY_ID_] = func;
    }

private:
    // 路由表: PacketID -> 处理函数
    std::map<u32, HandlerFunc> m_handlers;
};

ECCS_END