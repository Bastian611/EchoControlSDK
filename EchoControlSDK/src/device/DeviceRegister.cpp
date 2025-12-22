#include "DeviceFactory.h"
#include "DeviceID.h"
#include "DeviceBase.h" // 包含基类

// 引入具体设备头文件
#include "Light/Light_HL_525_4W/Light_HL_525_4W.h"
#include "PTZ/PTZ_YZ_BY010W/PTZ_YZ_BY010W.h"
#include "Sound/Sound_NetSpeaker_V2/Sound_NetSpeaker_V2.h"
#include "Ultrasonic/Ultrasonic_TAS_IO_428R2/Ultrasonic_TAS_IO_428R2.h"

#include <map>

ECCS_BEGIN

// --------------------------------------------------------------------------
// 全局映射表：ModelName (String) -> DeviceOID (u32)
// --------------------------------------------------------------------------
static std::map<str, u32> g_ModelToIDMap;

// 初始化映射关系的辅助函数
void InitModelMapping() {
    if (!g_ModelToIDMap.empty()) return;

    // Light
    g_ModelToIDMap["HL-525"] = Light_HL_525_4W::ID;

    // PTZ
    g_ModelToIDMap["YZ-BY010W"] = PTZ_YZ_BY010W::ID;

    // Sound
    g_ModelToIDMap["NETSPEAKER-V2"] = Sound_NetSpeaker_V2::ID;
}

// 实现 DeviceFactory.h 中声明的查找函数
u32 GetDeviceOIDByModelName(const str& modelName) {
    InitModelMapping(); 

    auto it = g_ModelToIDMap.find(modelName);
    if (it != g_ModelToIDMap.end()) {
        return it->second;
    }
    return DeviceID::INVALID_ID;
}

// --------------------------------------------------------------------------
// 工厂注册 (Key 是 u32 类型的 ID)
// --------------------------------------------------------------------------
FACTORY_BEGIN(u32, DeviceBase)

// 使用具体类的 ID 常量进行注册
FACTORY_APPEND(Light_HL_525_4W::ID,         Light_HL_525_4W,            DeviceBase)


FACTORY_APPEND(PTZ_YZ_BY010W::ID,           PTZ_YZ_BY010W,              DeviceBase)


FACTORY_APPEND(Sound_NetSpeaker_V2::ID,     Sound_NetSpeaker_V2,        DeviceBase)


FACTORY_APPEND(Ultrasonic_TAS_IO_428R2::ID, Ultrasonic_TAS_IO_428R2,    DeviceBase)

FACTORY_END(u32, DeviceBase)

ECCS_END