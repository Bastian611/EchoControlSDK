#pragma once
#include "../ILight_Device.h"
#include "../../../net/TCPSocket.h"

ECCS_BEGIN

class Light_HL_525_4W : public ILight_Device
{
public:
    
    // 注册ID
    static const u32 ID = MAKE_DEV_OID(did::DEVICE_LIGHT, did::LIGHT_HL_525_4W);
    // 注册工厂宏
    //FACTORY_ID_CHILD_WITH_SPEC_NAME(DeviceBase, Self, "HL-525")
    FACTORY_ID_CHILD_WITH_SPEC_NAME(u32, ID, DeviceBase, did::LIGHT_HL_525_4W, "HL-525")

    Light_HL_525_4W();
    virtual ~Light_HL_525_4W();

    // 覆盖初始化
    virtual bool Init(int slotID, const std::map<str, str>& config) override;

protected:
    // --- 实现 ILight_Device 的纯虚接口 ---
    virtual void SetSwitch(bool isOpen) override;
    virtual void SetBrightness(u8 level) override;
    virtual void SetStrobe(bool isOpen) override;

private:
    // 私有辅助方法
    void SendHexCmd(u8 cmd, u8 vh, u8 vl);
    bool Connect();

private:
    str m_ip;
    int m_port;
    TcpSocket_Ptr m_socket;
};

ECCS_END