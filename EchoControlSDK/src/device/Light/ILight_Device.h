#pragma once
#include "../DeviceBase.h"
#include "debug/Logger.h"

ECCS_BEGIN

class ILight_Device : public DeviceBase
{
public:
    
    virtual ECCS_Error SetLightSwitch(bool isOpen) = 0;
    virtual ECCS_Error SetLightLevel(u8 power) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support.",
            m_slotID, GetProperty("Model").c_str());
        return ECCS_ERR_NOT_SUPPORTED;
    }
    virtual ECCS_Error SetFlashFreq(u8 hz) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support.",
            m_slotID, GetProperty("Model").c_str());
        return ECCS_ERR_NOT_SUPPORTED;
    }
    virtual ECCS_Error SetBrightness(u8 level) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support.",
            m_slotID, GetProperty("Model").c_str());
        return ECCS_ERR_NOT_SUPPORTED;
    }
    virtual ECCS_Error SetWorkMode(RGB_V3_LightMode mode) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support.",
            m_slotID, GetProperty("Model").c_str());
        return ECCS_ERR_NOT_SUPPORTED;
    }
    virtual ECCS_Error SetFocus(RGB_V3_FocusType type, u16 value) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support.",
            m_slotID, GetProperty("Model").c_str());
        return ECCS_ERR_NOT_SUPPORTED;
    }
    /**
     * @brief 设置频闪 (扩展功能)
     * @param isOpen true=开启频闪, false=关闭
     */
    virtual ECCS_Error SetStrobe(bool isOpen) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support Strobe.",
            m_slotID, GetProperty("Model").c_str());
        return ECCS_ERR_NOT_SUPPORTED;
    }
};

ECCS_END