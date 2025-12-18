#pragma once
#include "../IPTZ_Device.h"
#include "../../../net/TCPSocket.h"

ECCS_BEGIN

class PTZ_YZ_BY010W : public IPTZ_Device
{
public:
    using Self = PTZ_YZ_BY010W;

    // 定义静态 ID 常量
    static const u32 ID = MAKE_DEV_OID(did::DEVICE_PTZ, did::PTZ_YZ_BY010W);

    // 使用 5 参数宏，Key 类型为 u32
    FACTORY_ID_CHILD_WITH_SPEC_NAME(u32, ID, DeviceBase, Self, "YZ-BY010W")

    PTZ_YZ_BY010W();
    virtual ~PTZ_YZ_BY010W();

    virtual bool Init(int slotID, const std::map<str, str>& config) override;

protected:
    virtual void PtzMove(u8 action, u8 speed) override;
    virtual void PtzStop() override;
    virtual void PtzPreset(u8 action, u8 index) override;

private:
    void SendPelcoD(u8 cmd1, u8 cmd2, u8 d1, u8 d2);
    bool Connect();

private:
    str m_ip;
    int m_port;
    u8  m_addr;
    TcpSocket_Ptr m_socket;
};

ECCS_END