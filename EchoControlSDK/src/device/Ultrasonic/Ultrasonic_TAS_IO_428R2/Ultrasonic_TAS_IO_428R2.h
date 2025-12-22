#pragma once
#include "../IUltrasonic_Device.h"
#include "net/TCPSocket.h"

ECCS_BEGIN

class Ultrasonic_TAS_IO_428R2 : public IUltrasonic_Device
{
public:
    using Self = Ultrasonic_TAS_IO_428R2;

    // 定义静态 ID 常量
    static const u32 ID = MAKE_DEV_OID(did::DEVICE_ULTRASONIC, did::ULTRASONIC_TAS_IO_428R2);

    // 注册工厂宏: Key=u32, Model Name="TAS-IO-428R2"
    FACTORY_ID_CHILD_WITH_SPEC_NAME(u32, ID, DeviceBase, Self, "TAS-IO-428R2")

    Ultrasonic_TAS_IO_428R2();
    virtual ~Ultrasonic_TAS_IO_428R2();

    // 覆盖初始化
    virtual bool Init(int slotID, const std::map<str, str>& config) override;

protected:
    // --- 实现 IUltrasonic_Device 接口 ---
    virtual void SetSwitch(u8 channel, bool isOpen) override;

private:
    // Modbus-TCP 组包发送
    void SendModbusCmd(u8 unitId, u16 addr, bool on);

    // 连接管理
    bool Connect();

private:
    str m_ip;
    int m_port;
    TcpSocket_Ptr m_socket;
};

ECCS_END