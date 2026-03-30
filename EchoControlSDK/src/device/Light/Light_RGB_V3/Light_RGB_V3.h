#pragma once
#include "../ILight_Device.h"
#include "../../DeviceDataTypes.h"
#include "net/TCPSocket.h"
#include <atomic>

ECCS_BEGIN


class Light_RGB_V3 : public ILight_Device
{
public:
    using Self = Light_RGB_V3;
    static const u32 ID = MAKE_DEV_OID(did::DEVICE_LIGHT, did::LIGHT_RGB_V3);
    FACTORY_ID_CHILD_WITH_SPEC_NAME(u32, ID, DeviceBase, Self, "RGB-V3")

    Light_RGB_V3();
    virtual ~Light_RGB_V3();

    virtual ECCS_Error Init(int slotID, const std::map<str, str>& config) override;
    virtual ECCS_Error Start() override;
    virtual ECCS_Error Stop() override;
    virtual bool Reconnect() override;

    // --- ILight_Device 接口实现 ---
    virtual ECCS_Error SetLightSwitch(bool isOpen) override; // 照明开关
    virtual ECCS_Error SetWorkMode(RGB_V3_LightMode mode) override; // 设置工作模式
    virtual ECCS_Error SetLightLevel(u8 power) override; // 设置照明功率
    virtual ECCS_Error SetFlashFreq(u8 hz) override; // 设置闪烁频率
    virtual ECCS_Error SetFocus(RGB_V3_FocusType type, u16 value) override; // 设置调焦参数
    virtual ECCS_Error SetStrobe(bool isOpen) override; // 频闪开关

protected:
    virtual int ReadRaw(u8* buf, u32 maxLen) override;
    virtual void OnRawDataReceived(const u8* data, u32 len) override;

private:
    ECCS_Error SendCommand();
    void ParseFeedback(const u8* data);
    bool Connect();

private:
    str m_ip;
    int m_port;
    TcpSocket_Ptr m_socket;

    // 指令状态缓存
    RGB_V3_LightMode m_curMode;
    u8  m_curPower; // 照明功率
    RGB_V3_FocusType m_curFocusType;
    u16 m_curFocusValue; // 调焦具体值
    u8  m_curFlashHz; // 闪烁频率，0代表连续输出，1-20表示频率

    // 回传状态
    std::atomic<float> m_devTemp{ 0.0f };
    std::atomic<float> m_warnTemp{ 0.0f };
    std::atomic<float> m_actualAngle{ 0.0f };
    std::atomic<float> m_curDist{ 0.0f };
    std::vector<u8> m_recvBuf;
};

ECCS_END