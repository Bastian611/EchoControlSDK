#pragma once
#include "IDevice.h"
#include "DeviceID.h"
#include "DeviceState.h"          
#include "DeviceEvents.h"          
#include "../config/PropertyManager.hpp"
#include "../thread/thread.h"
#include "../utils/factory.hpp"
#include "../debug/Logger.h"
#include "../protocol/Packet_Def.h" 
#include <functional>

ECCS_BEGIN

class Device : public IDevice, public Thread
{
public:
    Device() : Thread(), m_slotID(0), m_devState(DevState::STATE_OFFLINE) {}
    virtual ~Device() { Stop(); }

    FACTORY_BASE()

        // --- IDevice 接口实现 ---
        virtual bool Init(int slotID, const std::map<str, str>& config) override {
        m_slotID = slotID;

        // 注册通用属性
        m_props.Register<int>("Transform", 1, "Transform"); // 0：串口，1：网口
        m_props.Register<str>("IP", "127.0.0.1", "IP Address");
        m_props.Register<int>("Port", 8000, "Port");
        m_props.Register<int>("Baud", 115200, "Baudrate");
        m_props.Register<str>("Name", "Unnamed", "Device Name");
        m_props.Register<int>("ID", 0x00, "Device ID");
        m_props.Register<bool>("Enable", false, "Enable");

        OnRegisterProperties(); // 子类钩子
        m_props.Import(config);
        OnRegisterCommands();   // 子类钩子

        return true;
    }

    virtual bool Start() override {
        if (m_state == TS_RUNNING) return true;
        Thread::start();
        LOG_INFO("[Slot %d] Device Started.", m_slotID);
        return true;
    }

    virtual void Stop() override {
        Thread::quit();
        Thread::join();
        SetState(DevState::STATE_OFFLINE); // 线程停止即离线
        LOG_INFO("[Slot %d] Device Stopped.", m_slotID);
    }

    // [旧] 字符串指令 -> 放入 ExecuteEvent
    virtual void Execute(int cmdID, const str& args) override {
        auto e = std::make_shared<ExecuteEvent>(ExecuteData{ cmdID, args });
        postEvent(e);
    }

    // [新] Packet指令 -> 放入 PacketEvent
    virtual void ExecutePacket(std::shared_ptr<rpc::RpcPacket> pkt) override {
        if (pkt) {
            auto e = std::make_shared<PacketEvent>(pkt);
            postEvent(e);
        }
    }

    virtual str GetProperty(const str& key) const override {
        return m_props.GetString(key);
    }

    virtual bool IsOnline() const override {
        return m_devState == DevState::STATE_ONLINE || m_devState == DevState::STATE_WORKING;
    }

    DeviceID GetDeviceID() const { return m_deviceID; }

    // 状态回调
    using StatusCallback = std::function<void(std::shared_ptr<rpc::RpcPacket>)>;
    void SetStatusCallback(StatusCallback cb) { m_statusCb = cb; }

protected:
    // --- 供子类使用的钩子 ---
    virtual void OnRegisterProperties() {}
    virtual void OnRegisterCommands() {}

    // [关键] 子类处理 Packet
    virtual void OnPacketReceived(std::shared_ptr<rpc::RpcPacket> pkt) {}

    // [兼容] 旧的指令处理
    using CmdHandler = std::function<void(const str&)>;
    void RegisterCmd(int cmdID, CmdHandler handler) {
        m_dispatcher[cmdID] = handler;
    }

    // --- 状态管理 ---
    void SetState(DevState newState, int errCode = 0) {
        if (m_devState == newState) return;
        m_devState = newState;

        // 构造状态包并推送
        rpc::DeviceStatus status;
        status.deviceID = m_deviceID.Value();
        status.slotID = (u8)m_slotID;
        status.state = (u8)newState;
        status.errorCode = errCode;
        status.temperature = 0.0f;

        // 创建 OW 包
        auto pkt = std::make_shared<rpc::OwDeviceStatus>(status);
        if (m_statusCb) m_statusCb(pkt);

        // 也可以打印日志
         LOG_DEBUG("[Slot %d] State: %s", m_slotID, DevStateToStr(newState));
    }

    // --- 线程循环 ---
    virtual void run() override {
        while (m_state == TS_RUNNING) {
            Event_Ptr e = m_eq.pop(); // 阻塞等待
            if (!e) continue;

            if (e->eId() == EventTypes::Quit) break;

            // 1. 处理 Packet 事件 (推荐)
            if (e->eId() == DeviceEventID::PacketArrival) {
                auto pe = std::dynamic_pointer_cast<PacketEvent>(e);
                if (pe && pe->GetPacket()) {
                    OnPacketReceived(pe->GetPacket());
                }
            }
            // 2. 处理字符串指令事件 (兼容)
            else if (e->eId() == DeviceEventID::ExecuteString) {
                auto exe = std::dynamic_pointer_cast<ExecuteEvent>(e);
                if (exe) Dispatch(exe->Dat.cmdID, exe->Dat.args);
            }

            OnCustomEvent(e);
        }
    }

    virtual void OnCustomEvent(Event_Ptr& e) {}

private:
    void Dispatch(int cmdID, const str& args) {
        auto it = m_dispatcher.find(cmdID);
        if (it != m_dispatcher.end()) {
            try { it->second(args); }
            catch (std::exception& e) {
                LOG_ERROR("[Slot %d] Cmd %d Exception: %s", m_slotID, cmdID, e.what());
            }
        }
        else {
            LOG_WARNING("[Slot %d] Cmd %d Not Registered", m_slotID, cmdID);
        }
    }

protected:
    int m_slotID;
    DeviceID m_deviceID;
    PropertyManager m_props;
    DevState m_devState;
    StatusCallback m_statusCb;
    std::map<int, CmdHandler> m_dispatcher;
};

ECCS_END