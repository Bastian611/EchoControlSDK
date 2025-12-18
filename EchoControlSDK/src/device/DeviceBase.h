#pragma once
#include "../global.h"
#include "DeviceID.h"
#include "DeviceState.h"
#include "DeviceEvents.h"
#include "../thread/thread.h"
#include "../utils/factory.hpp"
#include "../protocol/Packet_Def.h"
#include <functional>
#include <map>
#include <sstream>

ECCS_BEGIN

// 内部使用的属性元数据结构
struct PropMeta {
    str key;
    str defaultValue;
    str description;
    // bool readOnly; // 可选：如果需要只读控制可加上
};

// 
class DeviceBase : public Thread
{
public:
    DeviceBase();
    virtual ~DeviceBase();

    // 工厂基类宏
    FACTORY_BASE()

    // ------------------------------------------------
    // 公共接口
    // ------------------------------------------------

    // 初始化：slotID 和 配置Map
    virtual bool Init(int slotID, const std::map<str, str>& config);

    // 生命周期控制
    virtual bool Start();
    virtual void Stop();

    // Packet 控制入口 (核心)
    virtual void ExecutePacket(std::shared_ptr<rpc::RpcPacket> pkt);

    // ------------------------------------------------
    // 属性与状态查询
    // ------------------------------------------------

    // 获取属性字符串值
    str GetProperty(const str& key) const;

    // 泛型获取属性值 (内置类型转换)
    template <typename T>
    T GetPropValue(const str& key) const {
        str valStr = GetProperty(key);
        if (valStr.empty()) return T(); // 返回默认零值
        std::stringstream ss(valStr);
        T val;
        ss >> val;
        return val;
    }

    // 判断是否在线
    virtual bool IsOnline() const;

    // 获取设备身份 ID
    DeviceID GetDeviceID() const;

    // 状态回调注册
    using StatusCallback = std::function<void(std::shared_ptr<rpc::RpcPacket>)>;
    void SetStatusCallback(StatusCallback cb);

protected:
    // ------------------------------------------------
    // 供子类使用的 API (Protected)
    // ------------------------------------------------

    // [新增] 注册属性
    template <typename T>
    void RegisterProp(const str& key, T defaultVal, const str& desc = "") {
        std::stringstream ss;
        ss << defaultVal;
        m_propMeta[key] = { key, ss.str(), desc };
        m_propValues[key] = ss.str(); // 设置初始默认值
    }

    // [状态] 切换状态并推送
    void SetState(DevState newState, int errCode = 0);

    // ------------------------------------------------
    // 子类虚函数钩子 (Hooks)
    // ------------------------------------------------

    // 子类在此注册特有属性
    virtual void OnRegisterProperties();

    // 子类在此注册指令 (如果用旧模式)
    virtual void OnRegisterCommands();

    // [关键] 子类处理收到的 Packet
    virtual void OnPacketReceived(std::shared_ptr<rpc::RpcPacket> pkt);

    // 自定义事件处理
    virtual void OnCustomEvent(Event_Ptr& e);

    // 线程循环实现
    virtual void run() override;

private:
    void Dispatch(int cmdID, const str& args);

protected:
    int m_slotID;
    DeviceID m_deviceID;
    DevState m_devState;
    StatusCallback m_statusCb;

    // 内置属性存储
    std::map<str, PropMeta> m_propMeta;   // 定义
    std::map<str, str>      m_propValues; // 值
};

ECCS_END