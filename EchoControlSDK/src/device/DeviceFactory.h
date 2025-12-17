#pragma once
#include "../global.h"
#include "../utils/factory.hpp"

ECCS_BEGIN

// 前置声明 Device 类，避免循环引用
class Device;

// 声明工厂：Key=std::string (Model Name), Base=Device
FACTORY_DECL_EXTERN(str, Device)

ECCS_END