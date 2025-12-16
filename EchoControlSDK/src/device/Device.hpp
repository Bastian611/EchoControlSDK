#pragma once
#include "IDevice.h"
#include "DeviceID.h"
#include "../config/PropertyManager.hpp"
#include "../thread/thread.h"
#include "../utils/factory.hpp"
#include "../debug/Logger.h"
#include <functional>

ECCS_BEGIN

class Device : public IDevice, public Thread
{
public:
    Device() : Thread(), m_slotID(0), m_deviceID(0) {}
    virtual ~Device() { Stop(); }

    // 工厂基类宏
    FACTORY_BASE()

        // --- IDevice 接口实现 ---
        virtual bool Init(int slotID, const std::map<str, str>& config) override {
        m_slotID = slotID;

        // 注册通用属性
        m_props.Register<int>("Transform", 1, "Transform"); // 0：串口，1：网口，2：其他
        m_props.Register<str>("IP", "127.0.0.1", "IP Address");
        m_props.Register<int>("Port", 8000, "Port");
        m_props.Register<int>("Baud", 115200, "Baudrate"); // 仅当Transform为串口时生效
        m_props.Register<str>("Name", "Unnamed", "Device Name");
        m_props.Register<int>("ID", 0x00, "Device ID");
        m_props.Register<bool>("Enable", false, "Enable");

        // 子类注册特有属性 (Hook)
        OnRegisterProperties();

        // 导入配置
        m_props.Import(config);

        // 子类注册指令处理函数 (Hook)
        OnRegisterCommands();

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
        LOG_INFO("[Slot %d] Device Stopped.", m_slotID);
    }

    virtual void Execute(int cmdID, const str& args) override {
        // 异步入队
        ExecuteEvent* e = new ExecuteEvent({ cmdID, args });
        postEvent(e);
    }

    virtual str GetProperty(const str& key) const override {
        return m_props.GetString(key);
    }

    // 获取 ID 供 SlotManager 校验
    DeviceID GetDeviceID() const { return m_deviceID; }

protected:
    // --- 供子类使用的钩子 ---
    virtual void OnRegisterProperties() {}
    virtual void OnRegisterCommands() {}

    // 注册指令处理 Handler
    using CmdHandler = std::function<void(const str&)>;
    void RegisterCmd(int cmdID, CmdHandler handler) {
        m_dispatcher[cmdID] = handler;
    }

    // --- 线程循环 ---
    virtual void run() override {
        // 可以在这里调用子类的连接逻辑
        // OnThreadStart(); 

        while (m_state == TS_RUNNING) {
            Event_Ptr e = m_eq.pop(); // 阻塞等待
            if (!e) continue;
            if (e->eId() == EventTypes::Quit) break;

            if (e->eId() == EventTypes::User + 1) { // ExecuteEvent
                auto exe = std::dynamic_pointer_cast<ExecuteEvent>(e);
                if (exe) Dispatch(exe->Dat.cmdID, exe->Dat.args);
            }

            // 可以处理自定义事件
            OnCustomEvent(e);
        }
    }

    virtual void OnCustomEvent(Event_Ptr& e) {}

private:
    void Dispatch(int cmdID, const str& args) {
        auto it = m_dispatcher.find(cmdID);
        if (it != m_dispatcher.end()) {
            try {
                it->second(args); // 执行 Lambda
            }
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
    DeviceID m_deviceID;     // 子类在 OnRegisterProperties 中赋值
    PropertyManager m_props; // 属性管家
    std::map<int, CmdHandler> m_dispatcher; // 路由表
};

ECCS_END