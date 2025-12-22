#pragma once
#include "../DeviceBase.h"

ECCS_BEGIN

class IUltrasonic_Device : public DeviceBase
{
public:
    // === 纯虚接口：由具体驱动实现 ===

    /**
     * @brief 设置开关状态
     * @param channel 通道号 (1=第1路, 2=第2路...)
     * @param isOpen  true=开, false=关
     */
    virtual void SetSwitch(u8 channel, bool isOpen) = 0;
};

ECCS_END