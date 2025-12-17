#pragma once
#include "../global.h"
#include "../utils/factory.hpp"

ECCS_BEGIN

// 前置声明 Device 类，避免循环引用
class DeviceBase;

// 声明工厂：Key=std::string (Model Name), Base=Device
FACTORY_DECL_EXTERN(u32, DeviceBase)

// 返回的是 OID (Type + Model, Index为0)
// 例如 "HL-525" -> 0x10020000
u32 GetDeviceOIDByModelName(const str& modelName);

ECCS_END