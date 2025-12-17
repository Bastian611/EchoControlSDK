#pragma once
#include "../global.h"
#include "../device/DeviceID.h" // 引用 did::DeviceType

ECCS_BEGIN
namespace rpc {

    // -------------------------------------------------------
    // ID 生成规则
    // -------------------------------------------------------
    // DevType: 1=Light, 2=Sound... (来自 did::DeviceType)
    // Cat:     1=OW, 2=Control, 3=Query, 4=Setting
    // IsRp:    0=Request, 1=Response
    // Idx:     序号

#define _MAKE_ID(devType, cmdType, isRp, idx) \
    (u32)( ((devType & 0xFF) << 24) | ((cmdType & 0xFF) << 16) | ((isRp & 0x01) << 15) | (idx & 0x7FFF) )

// -------------------------------------------------------
// 分类宏
// -------------------------------------------------------

// 1. OneWay (单向推送, Server -> Client)
#define _APP_OW_ID_(devType, idx)          _MAKE_ID(devType, 1, 0, idx)

// 2. Control (实时控制, 频率高)
#define _APP_RQ_CONTROL_ID_(devType, idx)  _MAKE_ID(devType, 2, 0, idx)
#define _APP_RP_CONTROL_ID_(devType, idx)  _MAKE_ID(devType, 2, 1, idx)

// 3. Query (状态查询, 频率中)
#define _APP_RQ_QUERY_ID_(devType, idx)    _MAKE_ID(devType, 3, 0, idx)
#define _APP_RP_QUERY_ID_(devType, idx)    _MAKE_ID(devType, 3, 1, idx)

// 4. Setting (参数配置, 频率低)
#define _APP_RQ_SETTING_ID_(devType, idx)  _MAKE_ID(devType, 4, 0, idx)
#define _APP_RP_SETTING_ID_(devType, idx)  _MAKE_ID(devType, 4, 1, idx)

}
ECCS_END