#pragma once
#include "../ISound_Device.h"
#include "net/TCPSocket.h"
#include "net/UDPSocket.h"
#include "utils/ring_buffer.h"
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

    virtual void SetVolume(u8 vol) override; 
    virtual void GetVolume(u8 vol_play, u8 vol_cap) override;

    virtual void PushAudio(const u8* data, u32 len) override; 

private:
    void SendJsonCmd(const str& json);
    str BuildJson(const char* cmd, const char* params = "");

    // 连接管理
    bool Connect();

    // 心跳线程函数
    void HeartbeatLoop();

    // 定义内部使用的心跳事件ID
    static const int EVENT_HEARTBEAT = EventTypes::User + 100;
    void AudioTxLoop(); 

private:
    str m_ip;
    int m_port;
    TcpSocket_Ptr m_socket;

    // 协议相关
    int m_cseq; // 自增序列号

    // 心跳专用
    std::thread* m_heartbeatThread;
    std::atomic<bool> m_keepHeartbeat;

    RingBuffer* m_audioBuf;
    std::thread* m_audioThread;
    bool m_isMicOpen;
};

ECCS_END