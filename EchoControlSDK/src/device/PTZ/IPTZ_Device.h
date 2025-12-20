#pragma once
#include "../DeviceBase.h"

ECCS_BEGIN

class IPTZ_Device : public DeviceBase
{
public:
    // === 纯虚接口 ===
    // action 定义参考 PacketDef.h (1=Up, 2=Down...)
    virtual void PtzMove(u8 action, u8 speed) = 0;
    virtual void PtzStop() = 0;
    virtual void PtzPreset(u8 action, u8 index) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support Preset.",
            m_slotID, GetProperty("Model").c_str());
    }
    virtual void PtzZoomIn() {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support ZoomIn.",
            m_slotID, GetProperty("Model").c_str());
    }
};

ECCS_END