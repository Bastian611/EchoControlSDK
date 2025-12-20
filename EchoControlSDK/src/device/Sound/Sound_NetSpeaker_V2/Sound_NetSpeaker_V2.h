#pragma once
#include "../ISound_Device.h"
#include "../../../net/TCPSocket.h"
#include <atomic>
#include <thread>

ECCS_BEGIN

class Sound_NetSpeaker_V2 : public ISound_Device
{
public:
    using Self = Sound_NetSpeaker_V2;
    // 定义 ID: Sound + NetSpeaker_V2
    static const u32 ID = MAKE_DEV_OID(did::DEVICE_SOUND, did::SOUND_NETSPEAKER_V2);

    // 注册工厂
    FACTORY_ID_CHILD_WITH_SPEC_NAME(u32, ID, DeviceBase, Self, "NetSpeaker-V2")

    Sound_NetSpeaker_V2();
    virtual ~Sound_NetSpeaker_V2();

    virtual bool Init(int slotID, const std::map<str, str>& config) override;
    virtual bool Start() override;
    virtual void Stop() override;

public:
    // --- 实现 ISound_Device 纯虚接口 ---
    virtual void PlayFile(const char* filename, bool loop) override;
    virtual void StopPlay() override;
    virtual void TTSPlay(const char* text) override;
    virtual void SetMic(bool isOpen) override;

    virtual void OnCustomEvent(Event_Ptr& e) override;

    // 我们需要在基类 DeviceBase 增加 SetVolume 接口，或者在 ISound_Device 中增加
    // 假设 ISound_Device 中有 SetVolume，这里补上实现
    // virtual void SetVolume(u8 vol) override; 

private:
    void SendJsonCmd(const str& json);
    str BuildJson(const char* cmd, const char* params = "");

    // 连接管理
    bool Connect();

    // 心跳线程函数
    void HeartbeatLoop();

    // 定义内部使用的心跳事件ID
    static const int EVENT_HEARTBEAT = EventTypes::User + 100;

private:
    str m_ip;
    int m_port;
    TcpSocket_Ptr m_socket;

    // 协议相关
    int m_cseq; // 自增序列号

    // 心跳专用
    std::thread* m_heartbeatThread;
    std::atomic<bool> m_keepHeartbeat;
};

ECCS_END