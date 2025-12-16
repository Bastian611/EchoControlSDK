#pragma once
#include "../global.h"
#include "DeviceID.h"
#include <map>
#include <string>

ECCS_BEGIN

// 前置声明
class DeviceID;

class IDevice {
public:
    virtual ~IDevice() = default;

    // 初始化 (传入配置 Map)
    virtual bool Init(int slotID, const std::map<std::string, std::string>& config) = 0;

    // 生命周期
    virtual bool Start() = 0;
    virtual void Stop() = 0;

    // 控制入口 (CmdID + Args)
    virtual void Execute(int cmdID, const std::string& args) = 0;

    // 属性与状态
    virtual bool IsOnline() const = 0;
    virtual std::string GetProperty(const std::string& key) const = 0;
};

ECCS_END