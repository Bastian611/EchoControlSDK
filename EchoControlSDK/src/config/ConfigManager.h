#pragma once
#include "../global.h"
#include "../utils/singleton.hpp"
#include "../device/DeviceBase.h"
#include <map>
#include <vector>

ECCS_BEGIN

// 槽位规则结构 (对应 system_rules.sys)
struct SlotRule {
    bool isMandatory;
    std::vector<str> allowedTypes; // 允许的设备类型字符串
};

// 负责加载系统配置并创建管理所有设备实例
class ConfigManager : public Singleton<ConfigManager>
{
    friend class Singleton<ConfigManager>;
    ConfigManager() = default;

public:
    ~ConfigManager();

    // 核心入口：加载规则和参数，创建设备
    void LoadSystem(const str& rulePath, const str& paramPath);

    // 获取运行时设备指针
    DeviceBase* GetDevice(int slotID);

    // 释放所有设备资源
    void Release();

private:
    // 内部辅助：解析 "Slot_1" -> 1
    int ParseSlotID(const str& sectionName);

private:
    // 存储槽位规则：SlotID -> Rule
    std::map<int, SlotRule> m_rules;

    // 存储活跃设备：SlotID -> Device Pointer
    // 这是系统唯一的设备持有者
    std::map<int, DeviceBase*> m_devices;
};

ECCS_END