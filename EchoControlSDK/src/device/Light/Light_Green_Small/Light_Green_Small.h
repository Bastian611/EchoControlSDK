#pragma once
#include "../ILight_Device.h"
#include "net/TCPSocket.h"

ECCS_BEGIN

class Light_Green_Small : public ILight_Device
{
public:
    using Self = Light_Green_Small;
    static const u32 ID = MAKE_DEV_OID(did::DEVICE_LIGHT, did::LIGHT_GREEN_SMALL);
    FACTORY_ID_CHILD_WITH_SPEC_NAME(u32, ID, DeviceBase, Self, "Green-Small")

    Light_Green_Small();
    virtual ~Light_Green_Small();

    virtual ECCS_Error Init(int slotID, const std::map<str, str>& config) override;
    virtual ECCS_Error Start() override;
    virtual ECCS_Error Stop() override;
    virtual bool Reconnect() override;

    // --- ILight_Device 接口实现 ---
    virtual ECCS_Error SetLightSwitch(bool isOpen) override;
    virtual ECCS_Error SetStrobe(bool isOpen) override;     // hz > 0 为开启，0 为关闭

protected:
    virtual int ReadRaw(u8* buf, u32 maxLen) override;

private:
    ECCS_Error SendFixedPacket(u8 cmd);
    bool Connect();

private:
    str m_ip;
    int m_port;
    TcpSocket_Ptr m_socket;
    bool m_bIsLighting;
};

ECCS_END