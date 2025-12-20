#pragma once
#include "../global.h"
#include "../utils/singleton.hpp"
#include "../utils/configparser.h"
#include <map>
#include <vector>

ECCS_BEGIN

// 前置声明，避免循环引用
class DeviceBase;

// 槽位规则结构
struct SlotRule {
    bool isMandatory;
    std::vector<str> allowedTypes;
};

// 负责加载系统配置、校验规则、创建并管理设备实例
class ConfigManager : public Singleton<ConfigManager>
{
    friend class Singleton<ConfigManager>;

private:

    ConfigManager();

public:
    ~ConfigManager();

    // 核心入口：加载规则(.sys) 和 参数(.dev)，创建设备
    void LoadSystem(const str& rulePath, const str& paramPath);

    // 获取运行时设备指针
    DeviceBase* GetDevice(int slotID);

    // 用于 ECCS_SetConfig 的接口
    bool UpdateConfig(int slotID, const str& key, const str& value);

    // 释放所有设备资源
    void Release();

    int GetDeviceCount() const { return m_devices.size(); }

    // 按索引获取 (线性扫描 map，虽然效率低但对于几十个设备无所谓，且保证顺序稳定)
    DeviceBase* GetDeviceByIndex(int index);

private:
    int ParseSlotID(const str& sectionName);
    // 保存参数文件路径，用于回写
    str m_paramPath;

private:
    std::map<int, SlotRule> m_rules;
    std::map<int, DeviceBase*> m_devices; // 系统中所有设备的持有者
};

ECCS_END