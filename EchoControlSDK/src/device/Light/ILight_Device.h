#pragma once
#include "../DeviceBase.h"
#include "debug/Logger.h"

ECCS_BEGIN

class ILight_Device : public DeviceBase
{
public:
    // === 纯虚接口：由具体驱动实现 ===
    virtual void SetSwitch(bool isOpen) = 0;
    virtual void SetBrightness(u8 level) = 0;
    /**
     * @brief 设置频闪 (扩展功能)
     * @param isOpen true=开启频闪, false=关闭
     * @note 默认实现为不支持。如果设备支持频闪，请在子类中重写此函数。
     */
    virtual void SetStrobe(bool isOpen) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support Strobe.",
            m_slotID, GetProperty("Model").c_str());
    }
};

ECCS_END