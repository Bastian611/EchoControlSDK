#include "DeviceFactory.h" // 包含工厂声明
#include "Light/Light_HL_525_4W/Light_HL_525_4W.h"
#include "PTZ/PTZ_YZ_BY010W/PTZ_YZ_BY010W.h"

ECCS_BEGIN

// -------------------------------------------------------------
// 驱动工厂注册表
// Key: 配置文件中的 Model 字符串
// Value: 创建函数
// -------------------------------------------------------------

FACTORY_BEGIN(str, Device)

// 强光
FACTORY_APPEND("HL-525", GreenLight, Device)

// 云台
FACTORY_APPEND("YZ-BY010W", YzPtz, Device)

// 后续添加更多...

FACTORY_END(str, Device)

ECCS_END