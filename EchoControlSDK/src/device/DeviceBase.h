#pragma once
#include "../global.h"
#include "DeviceID.h"
#include "DeviceState.h"
#include "DeviceEvents.h"
#include "../thread/thread.h"
#include "../utils/factory.hpp"
#include "../protocol/Packet_Def.h"
#include "../debug/Logger.h"
#include "../time/time_utils.h"
#include "../utils/buffer.h"
#include <functional>
#include <map>
#include <sstream>
#include <vector>

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

    // 供 SDK 内部使用，用于 ConfigManager 寻址
    int GetSlotID() const { return m_slotID; }

    // 状态回调注册
    using StatusCallback = std::function<void(std::shared_ptr<rpc::RpcPacket>)>;
    void SetStatusCallback(StatusCallback cb);

protected:
    // ------------------------------------------------
    // 供子类使用的 API (Protected)
    // ------------------------------------------------

    // 注册属性
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

    // 自定义事件处理
    virtual void OnCustomEvent(Event_Ptr& e);

    // 线程循环实现
    virtual void run() override;

    // 启动/停止读取线程 (供子类在 Start/Stop 中调用)
    void StartReader();
    void StopReader();

    // 原始数据接收钩子
    virtual void OnRawDataReceived(const u8* data, u32 len);

    // 底层读取接口 (子类必须实现，因为TCP/UDP/串口读取方式不同)
    // 返回读取的字节数，<=0 表示错误或超时
    virtual int ReadRaw(u8* buf, u32 maxLen) { return 0; }

private:
    // 读取线程函数
    void ReadLoop();

private:
    // 读取线程相关
    std::thread* m_readThread = nullptr;
    std::atomic<bool> m_keepReading = { false };

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