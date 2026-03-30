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

    // 生命周期
    virtual ECCS_Error Init(int slotID, const std::map<str, str>& config) override;
    virtual ECCS_Error Start() override;
    virtual ECCS_Error Stop() override;
    virtual bool Reconnect() override;

public:
    // 业务接口
    virtual ECCS_Error PtzMove(u8 action, u8 speed) override;
    virtual ECCS_Error PtzStop() override;
    virtual ECCS_Error PtzPreset(u8 action, u8 index) override;
    virtual ECCS_Error PtzReset() override;
    virtual ECCS_Error PtzSetAbsolutePos(float pan, float tilt) override;
    virtual ECCS_Error PtzQueryPosition() override;
    virtual ECCS_Error PtzZoom(bool isZoomIn) override;

    // --- 线扫角度设置
    virtual ECCS_Error PtzSetScanRange(float startAngle, float endAngle) override;
    // --- 启动线扫 ---
    virtual ECCS_Error PtzStartScan() override;
    // --- 停止线扫 ---
    virtual ECCS_Error PtzStopScan() override;

private:
    ECCS_Error SendPelcoD(u8 cmd1, u8 cmd2, u8 d1, u8 d2);
    bool Connect();
    // 辅助：解析角度包
    void ParseResponse(const u8* data);

protected:
    // 实现基类的 IO 接口
    virtual int ReadRaw(u8* buf, u32 maxLen) override;

    // 实现协议解析
    virtual void OnRawDataReceived(const u8* data, u32 len) override;

    virtual void OnStateEnter(DevState state) override;

private:
    str m_ip;
    int m_port;
    u8  m_addr;
    TcpSocket_Ptr m_socket;

    // 状态记录
    std::atomic<float> m_lastPan{ 0.0f };
    std::atomic<float> m_lastTilt{ 0.0f };
    std::vector<u8> m_recvBuf;
};

ECCS_END