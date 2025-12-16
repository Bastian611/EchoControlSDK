#pragma once
#include "../global.h"
#include <map>
#include <string>
#include <sstream>
#include <vector>

ECCS_BEGIN

// 属性元数据
struct PropertyMeta {
    str key;
    str defaultValue;
    str description;
    bool readOnly;
};

class PropertyManager {
public:
    // 注册属性 (模板方法)
    template <typename T>
    void Register(const str& key, T defaultVal, const str& desc = "", bool readOnly = false) {
        std::stringstream ss;
        ss << defaultVal;
        PropertyMeta meta = { key, ss.str(), desc, readOnly };
        m_metaMap[key] = meta;
        m_valueMap[key] = meta.defaultValue;
    }

    // 获取属性
    template <typename T>
    T Get(const str& key) const {
        auto it = m_valueMap.find(key);
        if (it == m_valueMap.end()) return T();
        std::stringstream ss(it->second);
        T val;
        ss >> val;
        return val;
    }

    // 特化 String 获取
    str GetString(const str& key) const {
        auto it = m_valueMap.find(key);
        return (it != m_valueMap.end()) ? it->second : "";
    }

    // 设置属性
    template <typename T>
    bool Set(const str& key, T val) {
        if (m_metaMap.find(key) == m_metaMap.end()) return false; // 未注册
        std::stringstream ss;
        ss << val;
        m_valueMap[key] = ss.str();
        return true;
    }

    // 批量加载 (从配置文件)
    void Import(const std::map<str, str>& config) {
        for (const auto& pair : config) {
            if (m_metaMap.count(pair.first)) { // 只导入已注册的属性
                m_valueMap[pair.first] = pair.second;
            }
        }
    }

private:
    std::map<str, PropertyMeta> m_metaMap;
    std::map<str, str> m_valueMap;
};

ECCS_END