#include "Sound_NetSpeaker_V2.h"
#include "debug/Logger.h"
#include "time/time_utils.h"
#include <chrono>

ECCS_BEGIN

// =================================================
// ctor / dtor
// =================================================

Sound_NetSpeaker_V2::Sound_NetSpeaker_V2()
{
}

Sound_NetSpeaker_V2::~Sound_NetSpeaker_V2()
{
    Stop();
}

// =================================================
// Init / Start / Stop
// =================================================

bool Sound_NetSpeaker_V2::Init(int slotID, const std::map<str, str>& config)
{
    if (!DeviceBase::Init(slotID, config))
        return false;

    m_ip = GetPropValue<str>("IP");
    m_port = GetPropValue<int>("Port");
    if (m_port == 0) m_port = 9527;

    return true;
}

bool Sound_NetSpeaker_V2::Start()
{
    DeviceBase::Start();
    Connect();
    return true;
}

void Sound_NetSpeaker_V2::Stop()
{
    Disconnect();
    DeviceBase::Stop();
}

// =================================================
// ISound_Device 接口实现
// =================================================

bool Sound_NetSpeaker_V2::SetSoundMode(SoundStatus mode)
{
    if (!IsOnline())
        return false;

    const char* model = "idle";
    switch (mode)
    {
    case SoundStatus::Idle:         model = "idle"; break;
    case SoundStatus::Player:       model = "player"; break;
    case SoundStatus::OneKey:       model = "one_key"; break;
    case SoundStatus::MicBroadcast: model = "mic_broadcast"; break;
    }

    char param[64];
    snprintf(param, sizeof(param), "\"model\":\"%s\"", model);
    SendJsonCmd(BuildJson("model_change", param));

    m_soundMode = mode;
    return true;
}

SoundStatus Sound_NetSpeaker_V2::GetSoundMode() const
{
    return m_soundMode;
}

// ---------- 播放 ----------

bool Sound_NetSpeaker_V2::PlayIndex(int index, bool loop)
{
    if (!SetSoundMode(SoundStatus::Player))
        return false;

    m_loopPlay = loop;

    char param[64];
    snprintf(param, sizeof(param),
        "\"index\":\"%d\",\"loop\":\"%d\"",
        index, loop ? 1 : 0);

    SendJsonCmd(BuildJson("start_play", param));
    SetState(STATE_WORKING);
    return true;
}

bool Sound_NetSpeaker_V2::StopPlay()
{
    SendJsonCmd(BuildJson("stop_play"));
    SetSoundMode(SoundStatus::Idle);
    SetState(STATE_ONLINE);
    return true;
}

bool Sound_NetSpeaker_V2::Next()
{
    SendJsonCmd(BuildJson("next"));
    return true;
}

bool Sound_NetSpeaker_V2::Prev()
{
    SendJsonCmd(BuildJson("prev"));
    return true;
}

// ---------- 一键驱散 ----------

bool Sound_NetSpeaker_V2::OneKeyPlay(int index)
{
    if (!SetSoundMode(SoundStatus::OneKey))
        return false;

    char param[64];
    snprintf(param, sizeof(param), "\"index\":\"%d\"", index);
    SendJsonCmd(BuildJson("one_key_play", param));

    SetState(STATE_WORKING);
    return true;
}

// ---------- 音量 ----------

bool Sound_NetSpeaker_V2::SetPlayVolume(u8 vol)
{
    char param[32];
    snprintf(param, sizeof(param), "\"vol\":\"%d\"", vol);
    SendJsonCmd(BuildJson("set_play_vol", param));
    m_playVolume = vol;
    return true;
}

bool Sound_NetSpeaker_V2::SetCaptureVolume(u8 vol)
{
    char param[32];
    snprintf(param, sizeof(param), "\"vol\":\"%d\"", vol);
    SendJsonCmd(BuildJson("set_cap_vol", param));
    m_captureVolume = vol;
    return true;
}

// ---------- 实时音频 ----------

bool Sound_NetSpeaker_V2::PushAudio(const u8* data, u32 len)
{
    if (m_soundMode != SoundStatus::MicBroadcast)
        return false;

    if (m_audioTxBuf)
        m_audioTxBuf->Write(data, len);

    return true;
}

// =================================================
// DeviceBase 状态机钩子
// =================================================

void Sound_NetSpeaker_V2::OnStateEnter(DevState state)
{
    if (state == STATE_ONLINE)
    {
        // 心跳线程
        m_keepHeartbeat = true;
        m_heartbeatThread =
            new std::thread(&Sound_NetSpeaker_V2::HeartbeatLoop, this);

        // UDP 音频线程
        m_audioTxBuf = new RingBuffer(1024 * 100);
        m_keepAudioTx = true;
        m_audioTxThread =
            new std::thread(&Sound_NetSpeaker_V2::AudioTxLoop, this);
    }
}

void Sound_NetSpeaker_V2::OnStateExit(DevState state)
{
    if (state == STATE_ONLINE)
    {
        // 停心跳
        m_keepHeartbeat = false;
        if (m_heartbeatThread)
        {
            m_heartbeatThread->join();
            delete m_heartbeatThread;
            m_heartbeatThread = nullptr;
        }

        // 停音频
        m_keepAudioTx = false;
        if (m_audioTxThread)
        {
            m_audioTxThread->join();
            delete m_audioTxThread;
            m_audioTxThread = nullptr;
        }

        delete m_audioTxBuf;
        m_audioTxBuf = nullptr;
    }
}

// =================================================
// 网络 / 协议
// =================================================

bool Sound_NetSpeaker_V2::Connect()
{
    if (m_ctrlSocket && m_ctrlSocket->isOpen())
        return true;

    try
    {
        SetState(STATE_CONNECTING);

        m_ctrlSocket = std::make_shared<TcpSocket>(m_ip, m_port);
        m_ctrlSocket->setConnTimeout(1000);
        m_ctrlSocket->open();

        SetState(STATE_ONLINE);
        return true;
    }
    catch (...)
    {
        SetState(STATE_OFFLINE);
        return false;
    }
}

void Sound_NetSpeaker_V2::Disconnect()
{
    if (m_ctrlSocket)
        m_ctrlSocket->close();
}

// =================================================
// JSON / 线程
// =================================================

str Sound_NetSpeaker_V2::BuildJson(const char* cmd, const char* params)
{
    char buf[512];
    int seq = ++m_cseq;

    if (params)
        snprintf(buf, sizeof(buf),
            "{\"command\":\"%s\",\"cseq\":\"%d\",%s}\r\n\r\n",
            cmd, seq, params);
    else
        snprintf(buf, sizeof(buf),
            "{\"command\":\"%s\",\"cseq\":\"%d\"}\r\n\r\n",
            cmd, seq);

    return str(buf);
}

void Sound_NetSpeaker_V2::SendJsonCmd(const str& json)
{
    if (!Connect())
        return;

    try
    {
        m_ctrlSocket->write((const u8*)json.c_str(), json.size());
    }
    catch (...)
    {
        SetState(STATE_ERROR);
        m_ctrlSocket->close();
    }
}

// ---------- 心跳 ----------

void Sound_NetSpeaker_V2::HeartbeatLoop()
{
    while (m_keepHeartbeat)
    {
        msleep(30000);

        if (IsOnline())
        {
            SendJsonCmd(BuildJson("online"));
        }
    }
}

// ---------- UDP 音频 ----------

void Sound_NetSpeaker_V2::AudioTxLoop()
{
    UdpSocket udp(m_ip, 9888);
    try { udp.open(); }
    catch (...) { return; }

    u8 buf[1024];

    while (m_keepAudioTx)
    {
        int len = m_audioTxBuf->Read(buf, sizeof(buf));
        if (len > 0)
            udp.write(buf, len);
        else
            msleep(10);
    }
}

ECCS_END
