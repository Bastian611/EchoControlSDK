#pragma once
#include "../IPTZ_Device.h"
#include "net/TCPSocket.h"
#include "debug/Logger.h"

ECCS_BEGIN

class PTZ_YZ_BY010W : public IPTZ_Device
{
public:

    using Self = PTZ_YZ_BY010W; // 使用别名，避免编译出错
    // 定义静态 ID 常量
    static const u32 ID = MAKE_DEV_OID(did::DEVICE_PTZ, did::PTZ_YZ_BY010W);

    // 使用 5 参数宏，Key 类型为 u32
    FACTORY_ID_CHILD_WITH_SPEC_NAME(u32, ID, DeviceBase, Self, "YZ-BY010W")

    PTZ_YZ_BY010W();
    virtual ~PTZ_YZ_BY010W();

    virtual bool Init(int slotID, const std::map<str, str>& config) override;

public:
    virtual void PtzMove(u8 action, u8 speed) override;
    virtual void PtzStop() override;
    virtual void PtzPreset(u8 action, u8 index) override;

private:
    void SendPelcoD(u8 cmd1, u8 cmd2, u8 d1, u8 d2);
    bool Connect();

protected:
    // 实现基类的 IO 接口
    virtual int ReadRaw(u8* buf, u32 maxLen) override;

    // 实现协议解析
    virtual void OnRawDataReceived(const u8* data, u32 len) override;

    // 覆盖 Start/Stop 以启动读取线程
    virtual bool Start() override;
    virtual void Stop() override;

private:
    // 辅助：解析角度包
    void ParsePelcoResponse(const u8* data, u32 len);

private:
    str m_ip;
    int m_port;
    u8  m_addr;
    TcpSocket_Ptr m_socket;
};

ECCS_END