#include "ConfigManager.h"
#include "../device/DeviceBase.h"     
#include "../device/DeviceFactory.h"  
#include "../device/DeviceID.h"
#include "../utils/configparser.h"
#include "../debug/Logger.h"
#include "../debug/Exceptions.h"
#include "../utils/utils.h"
#include <cstdlib>

ECCS_BEGIN

ConfigManager::~ConfigManager() {
    Release();
}

void ConfigManager::Release() {
    for (auto& pair : m_devices) {
        if (pair.second) {
            pair.second->Stop();
            delete pair.second;
        }
    }
    m_devices.clear();
}

DeviceBase* ConfigManager::GetDevice(int slotID) {
    auto it = m_devices.find(slotID);
    return (it != m_devices.end()) ? it->second : nullptr;
}

int ConfigManager::ParseSlotID(const str& sectionName) {
    if (sectionName.find("Slot_") != 0) return -1;
    str numStr = sectionName.substr(5);
    return std::atoi(numStr.c_str());
}

void ConfigManager::LoadSystem(const str& rulePath, const str& paramPath)
{
    LOG_INFO("ConfigManager: Loading System... Rules: %s, Params: %s", rulePath.c_str(), paramPath.c_str());

    // 加载规则 (system_rules.sys)
    ConfigParser::ConfigParser ruleParser(rulePath);
    if (ruleParser.parseConfigFile()) {
        auto ruleSections = ruleParser.GetAllSections();
        for (const auto& sec : ruleSections) {
            int id = ParseSlotID(sec);
            if (id < 0) continue;

            SlotRule rule;
            str mandatoryStr = ruleParser.Get(sec, "Mandatory");
            rule.isMandatory = (mandatoryStr == "true" || mandatoryStr == "1");

            str types = ruleParser.Get(sec, "AllowedTypes");
            rule.allowedTypes = split(types, ',');
            m_rules[id] = rule;
        }
    }

    // 加载参数 (device_params.dev)
    ConfigParser::ConfigParser devParser(paramPath);
    if (!devParser.parseConfigFile()) {
        throw EIOException("Failed to load param file: %s", paramPath.c_str());
    }

    auto sections = devParser.GetAllSections();
    for (const auto& secName : sections) {
        int slotID = ParseSlotID(secName);
        if (slotID < 0) continue;

        // 检查启用状态
        str enableStr = devParser.Get(secName, "Enable");
        if (enableStr != "true" && enableStr != "1") {
            if (m_rules[slotID].isMandatory) {
                LOG_ERROR("[ConfigManager] Slot %d is Mandatory but Disabled!", slotID);
            }
            continue;
        }

        // 读取关键信息
        str modelName = devParser.Get(secName, "Model");
        str idStr = devParser.Get(secName, "ID");

        if (modelName.empty() || idStr.empty()) {
            LOG_ERROR("[ConfigManager] Slot %d Missing Model or ID.", slotID);
            continue;
        }

        // --- 核心校验逻辑 ---

        // 查表获取标准 OID
        u32 expectedOID = GetDeviceOIDByModelName(modelName);
        if (expectedOID == DeviceID::INVALID_ID) {
            LOG_ERROR("[ConfigManager] Slot %d Unknown Model: %s", slotID, modelName.c_str());
            continue;
        }

        // 解析配置 ID
        u32 configFullID = std::strtoul(idStr.c_str(), nullptr, 16);

        // 双向验证 (Model vs ID)
        if (expectedOID != DeviceID::GetOID(configFullID)) {
            LOG_ERROR("[ConfigManager] Slot %d ID Mismatch! Model(%s) != ID(%s)", slotID, modelName.c_str(), idStr.c_str());
            continue;
        }

        // Index 验证
        if (DeviceID::GetIndex(configFullID) < 1) {
            LOG_ERROR("[ConfigManager] Slot %d Invalid Index in ID %s (Must >= 1)", slotID, idStr.c_str());
            continue;
        }

        // --- 创建设备 ---
        DeviceBase* dev = FACTORY_CREATE(expectedOID, DeviceBase);

        if (dev) {
            // 构造配置 Map
            //std::map<str, str> confMap;
            // 手动提取通用属性
            //confMap["ID"] = idStr;
            //confMap["IP"] = devParser.Get(secName, "IP");
            //confMap["Port"] = devParser.Get(secName, "Port");
            //confMap["Name"] = devParser.Get(secName, "Name");
            //confMap["Baud"] = devParser.Get(secName, "Baud");
            // 传入 Enable 状态
            //confMap["Enable"] = enableStr;
            // 自动提取通用属性
            std::map<str, str> confMap = devParser.GetSection(secName);

            if (dev->Init(slotID, confMap)) {
                dev->Start();
                m_devices[slotID] = dev;
                LOG_INFO("[ConfigManager] Device Created: Slot %d, Model %s", slotID, modelName.c_str());
            }
            else {
                LOG_ERROR("[ConfigManager] Slot %d Init Failed.", slotID);
                delete dev;
            }
        }
        else {
            LOG_ERROR("[ConfigManager] Factory Failed for OID 0x%X", expectedOID);
        }
    }
}

ECCS_END