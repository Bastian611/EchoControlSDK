#pragma once
#include "../ILight_Device.h"
#include "net/TCPSocket.h"
#include "debug/Logger.h"

ECCS_BEGIN

class Light_HL_525_4W : public ILight_Device
{
public:

    using Self = Light_HL_525_4W;
    // 注册ID
    static const u32 ID = MAKE_DEV_OID(did::DEVICE_LIGHT, did::LIGHT_HL_525_4W);
    // 注册工厂宏
    FACTORY_ID_CHILD_WITH_SPEC_NAME(u32, ID, DeviceBase, Self, "HL-525")

    Light_HL_525_4W();
    virtual ~Light_HL_525_4W();

    // 覆盖初始化
    virtual ECCS_Error Init(int slotID, const std::map<str, str>& config) override;

public:
    virtual ECCS_Error SetLightSwitch(bool isOpen) override;
    virtual ECCS_Error SetBrightness(u8 level) override;
    virtual ECCS_Error SetStrobe(bool isOpen) override;

private:
    // 私有辅助方法
    ECCS_Error SendHexCmd(u8 cmd, u8 vh, u8 vl);
    bool Connect();

private:
    str m_ip;
    int m_port;
    TcpSocket_Ptr m_socket;
};

ECCS_END