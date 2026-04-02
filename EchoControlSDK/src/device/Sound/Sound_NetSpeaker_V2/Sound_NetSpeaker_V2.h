#pragma once
#include "../ISound_Device.h"
#include "net/TCPSocket.h"
#include "net/UDPSocket.h"
#include "utils/ring_buffer.h"
#include "device/DeviceDataTypes.h"
#include <atomic>
#include <thread>
#include <vector>

ECCS_BEGIN

class Sound_NetSpeaker_V2 : public ISound_Device
{
public:
    using Self = Sound_NetSpeaker_V2;

    // Sound + NetSpeaker_V2
    static const u32 ID = MAKE_DEV_OID(did::DEVICE_SOUND, did::SOUND_NETSPEAKER_V2);

    FACTORY_ID_CHILD_WITH_SPEC_NAME(u32, ID, DeviceBase, Self, "NetSpeaker-V2")

    Sound_NetSpeaker_V2();
    virtual ~Sound_NetSpeaker_V2();

    // =================================================
    // Device 生命周期
    // =================================================
    virtual ECCS_Error Init(int slotID, const std::map<str, str>& config) override;
    virtual ECCS_Error Start() override;
    virtual ECCS_Error Stop() override;

    // =================================================
    // ISound_Device 接口实现（最终定稿语义）
    // =================================================

    // --- 模式 ---
    virtual bool SetSoundMode(SoundStatus mode) override;
    virtual SoundStatus GetSoundMode() const override;
    virtual ECCS_Error QuerySoundStatus(SoundStatusData ss) override;
    virtual ECCS_Error CheckAudioIndex(int index) override;

    // --- 播放控制 ---
    virtual ECCS_Error PlayIndex(int index, bool loop = false) override;
    virtual ECCS_Error StopPlay() override;
    virtual ECCS_Error Next() override;
    virtual ECCS_Error Prev() override;

    virtual ECCS_Error MicSwitch(bool isOpen) override;
    virtual ECCS_Error SetTalk(bool isOpen) override;

    // --- 一键驱散 ---
    virtual ECCS_Error OneKeyPlay(int index) override;

    // --- 音量 ---
    virtual ECCS_Error SetPlayVolume(u8 vol) override;
    virtual ECCS_Error GetPlayVolume(SoundVolume& sv) override;
    virtual ECCS_Error SetCaptureVolume(u8 vol) override;

    virtual ECCS_Error GetAudioList(SoundAudioList& list) override;

    virtual ECCS_Error UploadAudioFile(const str& name, const u8* data, u32 len) override;

    virtual ECCS_Error DeleteAudioFile(int index) override;

    // --- 实时音频 ---
    // MicBroadcast 模式下调用
    virtual ECCS_Error PushAudio(const u8* data, u32 len) override;

protected:
    // =================================================
    // DeviceBase 钩子
    // =================================================
    virtual void OnCustomEvent(Event_Ptr& e) override;
    virtual void OnStateEnter(DevState state) override;
    virtual void OnStateExit(DevState state) override;
    virtual void OnRegisterProperties() override;

    // =================================================
    // 内部协议 & 连接管理
    // =================================================
    bool Connect();
    void Disconnect();

    bool SendJsonCmd(const char* cmd, const char* params, int timeout_ms = 2000);
    str  BuildJson(const char* cmd, const char* params, int seq);
    void CtrlRxLoop();
    void HandleJsonReply(const str& json);

    // =================================================
    // 后台线程
    // =================================================
    void HeartbeatLoop();   // TCP 心跳
    void AudioTxLoop();     // SDK → 设备（UDP）
    void AudioRxLoop();     // 设备 → SDK（UDP，下行音频）

    // =================================================
    // 设备能力 & 配置同步
    // =================================================
    bool QueryDeviceInfo();         // 连接后获取设备信息
    bool QueryVolume();             // 从设备读取音量
    bool QueryAudioList();          // 获取音频列表
    bool SyncConfigFromDevice();    // 写入 config
    bool SyncConfigToDevice();      // 应用 config

    void UpdateAudioSpec(SoundStatus mode);

private:
    // =================================================
    // 网络
    // =================================================
    str             m_ip;
    int             m_port{ 0 };
    TcpSocket_Ptr   m_ctrlSocket;
    UdpSocket_Ptr   m_audioTxSocket;
    UdpSocket_Ptr   m_audioRxSocket;

    // =================================================
    // 协议状态
    // =================================================
    std::atomic<int>            m_cseq{ 0 };
    std::atomic<SoundStatus>    m_soundMode{ SoundStatus::Idle };
    SoundStatusData             m_lastPushedSoundData; // 记录上次推送的快照
    std::atomic<bool>           m_loopPlay{ false };

    // =================================================
    // 音频缓存（SDK → 设备）
    // =================================================
    RingBuffer* m_audioTxBuf{ nullptr };
    std::thread* m_audioTxThread{ nullptr };
    std::atomic<bool>  m_keepAudioTx{ false };

    // =================================================
    // 下行音频（设备 → SDK）
    // =================================================
    std::thread* m_audioRxThread{ nullptr };
    std::atomic<bool>  m_keepAudioRx{ false };

    // 动态端口相关
    std::atomic<int> m_txTargetPort{ 8999 }; // 目标设备端口：PCM=8999, MP3=9888
    std::atomic<int> m_rxLocalPort{ 11200 }; // 本地绑定端口：用于接收设备发回的音频
    AudioSpec m_currentSpec;
    mutable std::mutex m_specMutex;
    AudioSpec GetCurrentSpec() const;

    // =================================================
    // 心跳
    // =================================================
    std::thread* m_heartbeatThread{ nullptr };
    std::atomic<bool>  m_keepHeartbeat{ false };

    // 控制通道接收
    std::thread* m_ctrlRxThread{ nullptr };
    std::atomic<bool> m_flagReadCmd{ false };
    std::string m_ctrlRxCache;

    // =================================================
    // 设备信息缓存
    // =================================================
    u8                m_playVolume{ 0 };
    u8                m_captureVolume{ 0 };
    std::vector<SoundFileInfo>  m_audioList;

    Semaphore m_ackSem;
    std::atomic<int> m_waitingCseq{ -1 }; // 正在等待回复的序列号
    bool m_lastAckResult{ false };   // 暂存回复结果
    std::recursive_mutex m_cmdMtx;
    int m_lastPlayIdx{ -1 };
};

ECCS_END
