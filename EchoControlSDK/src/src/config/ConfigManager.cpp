#include "ConfigManager.h"
#include "../utils/configparser.h" 
#include "../device/DeviceFactory.h"
#include "../device/DeviceID.h"
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

    // -------------------------------------------------------------
    // 加载规则 (system_rules.sys)
    // -------------------------------------------------------------
    // 格式示例:
    // [Slot_1]
    // Mandatory=true
    // AllowedTypes=Light,PTZ
    // -------------------------------------------------------------
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
            // 使用 utils.h 中的 split 函数分割逗号
            rule.allowedTypes = split(types, ',');

            m_rules[id] = rule;
        }
    }
    else {
        LOG_WARNING("ConfigManager: Rule file not found or empty, skipping rules.");
    }

    // -------------------------------------------------------------
    // 加载参数并创建设备 (device_params.dev)
    // -------------------------------------------------------------
    ConfigParser::ConfigParser devParser(paramPath);
    if (!devParser.parseConfigFile()) {
        throw EIOException("Failed to load param file: %s", paramPath.c_str());
    }

    // 遍历所有可能的 Slot (这里以 ConfigParser 解析到的 Section 为准)
    auto sections = devParser.GetAllSections();

    // 如果有规则，也可以遍历规则来检查 Mandatory 缺失
    // 这里采用遍历配置文件的方式
    for (const auto& secName : sections) {
        int slotID = ParseSlotID(secName);
        if (slotID < 0) continue;

        // 检查是否启用
        str enableStr = devParser.Get(secName, "Enable");
        if (enableStr != "true" && enableStr != "1") {
            // 如果是强制槽位但未启用，报警
            if (m_rules[slotID].isMandatory) {
                LOG_ERROR("[Slot %d] Mandatory slot is disabled or missing!", slotID);
            }
            continue;
        }

        // 读取关键信息
        str modelName = devParser.Get(secName, "Model"); // "HL-525"
        str idStr = devParser.Get(secName, "ID");    // "0x10020001"

        if (modelName.empty() || idStr.empty()) {
            LOG_ERROR("[Slot %d] Configuration Error: Missing 'Model' or 'ID'", slotID);
            continue;
        }

        // =========================================================
        // [核心逻辑] ID 双向验证 & 索引检查
        // =========================================================

        // 查表获取 Model 对应的标准 OID
        u32 expectedOID = GetDeviceOIDByModelName(modelName);
        if (expectedOID == DeviceID::INVALID_ID) {
            LOG_ERROR("[Slot %d] Unknown Model Name: %s", slotID, modelName.c_str());
            continue;
        }

        // 解析配置文件中的 ID
        u32 configFullID = std::strtoul(idStr.c_str(), nullptr, 16);
        u32 configOID = DeviceID::GetOID(configFullID);
        u8  configIndex = DeviceID::GetIndex(configFullID);

        // 验证：Model 对应的 OID 必须等于 配置 ID 的 OID
        if (expectedOID != configOID) {
            LOG_ERROR("[Slot %d] ID Mismatch! Model '%s' (0x%X) != ConfigID (0x%X)",
                slotID, modelName.c_str(), expectedOID, configOID);
            continue;
        }

        // 验证：Index 必须 >= 1
        if (configIndex < 1) {
            LOG_ERROR("[Slot %d] Invalid Index! ID 0x%08X has index 0.", slotID, configFullID);
            continue;
        }

        // =========================================================
        // 创建与初始化
        // =========================================================

        // 使用 OID (类型ID) 创建对象
        DeviceBase* dev = FACTORY_CREATE(expectedOID, DeviceBase);

        if (dev) {
            // 构造 Config Map
            std::map<str, str> confMap;

            // 暂时 ConfigParser 没有 GetSectionMap 接口，手动填充常用项
            // 或者修改 ConfigParser 增加 GetSection 接口
            confMap["ID"] = idStr;
            confMap["IP"] = devParser.Get(secName, "IP");
            confMap["Port"] = devParser.Get(secName, "Port");
            confMap["Name"] = devParser.Get(secName, "Name");
            confMap["Baud"] = devParser.Get(secName, "Baud");
            // ... 其他需要的属性 ...

            if (dev->Init(slotID, confMap)) {
                dev->Start();
                m_devices[slotID] = dev;
                LOG_INFO("[Slot %d] Device Loaded: %s (ID: %s)", slotID, modelName.c_str(), idStr.c_str());
            }
            else {
                LOG_ERROR("[Slot %d] Device Init failed.", slotID);
                delete dev;
            }
        }
        else {
            LOG_ERROR("[Slot %d] Factory create failed for Model: %s", slotID, modelName.c_str());
        }
    }
}

ECCS_END