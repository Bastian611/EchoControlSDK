#pragma once
#include "../global.h"
#include "DeviceID.h"
#include "DeviceState.h"
#include "DeviceEvents.h"
#include "../config/ConfigManager.h"
#include "../thread/thread.h"
#include "../utils/factory.hpp"
#include "../debug/Logger.h"
#include "../protocol/Packet_Def.h"
#include <functional>
#include <map>

ECCS_BEGIN

// 使用 DeviceBase 避免与 Windows 系统库冲突
class DeviceBase : public Thread
{
public:
    DeviceBase() : Thread(), m_slotID(0), m_devState(STATE_OFFLINE) {}
    virtual ~DeviceBase() { Stop(); }

    // 工厂基类宏
    FACTORY_BASE()

    // ------------------------------------------------
    // 公共接口
    // ------------------------------------------------

    // 初始化
    virtual bool Init(int slotID, const std::map<str, str>& config) {
    m_slotID = slotID;

    // 注册通用属性
    m_props.Register<int>("Transform", 1, "Transform"); // 0:Serial, 1:Net
    m_props.Register<str>("IP", "127.0.0.1", "IP Address");
    m_props.Register<int>("Port", 8000, "Port");
    m_props.Register<str>("Type", "Unnamed", "Device Type");
    m_props.Register<str>("Model", "Unnamed", "Device Model");
    m_props.Register<int>("Index", 1, "Device Index");
    m_props.Register<str>("ID", "0x00000000", "Device ID");
    m_props.Register<bool>("Enable", false, "Enable");

    OnRegisterProperties(); // 子类钩子
    m_props.Import(config);

    // 从属性中解析出 ID 并赋值给成员变量
    str idStr = m_props.GetString("ID");
    u32 fullID = std::strtoul(idStr.c_str(), nullptr, 16);

    m_deviceID = DeviceID(fullID);

    // 二次校验（对象内部自检）
    if (!m_deviceID.IsIndexValid()) {
        LOG_ERROR("Device Init: Invalid Index 0");
        return false;
    }

    OnRegisterCommands();

    return true;
    }

    // 启动
    virtual bool Start() {
        if (m_state == TS_RUNNING) return true;
        Thread::start();
        LOG_INFO("[Slot %d] Device Started.", m_slotID);
        return true;
    }

    // 停止
    virtual void Stop() {
        Thread::quit();
        Thread::join();
        SetState(STATE_OFFLINE);
        LOG_INFO("[Slot %d] Device Stopped.", m_slotID);
    }

    // Packet 控制入口
    virtual void ExecutePacket(std::shared_ptr<rpc::RpcPacket> pkt) {
        if (pkt) {
            // 使用 new 创建裸指针，交给 Thread 接管
            PacketEvent* e = new PacketEvent(pkt);
            postEvent(e);
        }
    }

    // 属性获取
    virtual str GetProperty(const str& key) const {
        return m_props.GetString(key);
    }

    // 在线状态
    virtual bool IsOnline() const {
        return m_devState == STATE_ONLINE || m_devState == STATE_WORKING;
    }

    DeviceID GetDeviceID() const { return m_deviceID; }

    // 状态回调
    using StatusCallback = std::function<void(std::shared_ptr<rpc::RpcPacket>)>;
    void SetStatusCallback(StatusCallback cb) { m_statusCb = cb; }

protected:
    // ------------------------------------------------
    // 供子类实现的钩子
    // ------------------------------------------------
    virtual void OnRegisterProperties() {}

    virtual void OnRegisterCommands() {}

    // [关键] 子类处理 Packet
    virtual void OnPacketReceived(std::shared_ptr<rpc::RpcPacket> pkt) {}

    // [兼容] 旧指令处理
    using CmdHandler = std::function<void(const str&)>;
    void RegisterCmd(int cmdID, CmdHandler handler) {
        m_dispatcher[cmdID] = handler;
    }

    // ------------------------------------------------
    // 内部功能函数
    // ------------------------------------------------
    void SetState(DevState newState, int errCode = 0) {
        if (m_devState == newState) return;
        m_devState = newState;

        // 构造状态包
        rpc::DeviceStatus status;
        status.deviceID = m_deviceID.Value();
        status.slotID = (u8)m_slotID;
        status.state = (u8)newState;
        status.errorCode = errCode;
        status.temperature = 0.0f;

        auto pkt = std::make_shared<rpc::OwDeviceStatus>(status);
        if (m_statusCb) m_statusCb(pkt);

        // LOG_DEBUG("[Slot %d] State: %s", m_slotID, DevStateToStr(newState));
    }

    // 线程循环
    virtual void run() override {
        while (m_state == TS_RUNNING) {
            Event_Ptr e = m_eq.pop();
            if (!e) continue;

            if (e->eId() == EventTypes::Quit) break;

            if (e->eId() == DeviceEventID::PacketArrival) {
                auto pe = std::dynamic_pointer_cast<PacketEvent>(e);
                if (pe && pe->GetPacket()) {
                    OnPacketReceived(pe->GetPacket());
                }
            }
            else if (e->eId() == DeviceEventID::ExecuteString) {
                auto exe = std::dynamic_pointer_cast<ExecuteEvent>(e);
                if (exe) Dispatch(exe->Dat.cmdID, exe->Dat.args);
            }
        }
    }

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
    ConfigManager m_props;
    DevState m_devState;
    StatusCallback m_statusCb;
    std::map<int, CmdHandler> m_dispatcher;
};

ECCS_END