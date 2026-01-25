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

ECCS_Error Sound_NetSpeaker_V2::Init(int slotID, const std::map<str, str>& config)
{
    if (!DeviceBase::Init(slotID, config))
        return ECCS_ERR_NOT_INIT;

    m_ip = GetPropValue<str>("IP");
    m_port = GetPropValue<int>("Port");
    if (m_port == 0) m_port = 9527;

    return ECCS_SUCCESS;
}

ECCS_Error Sound_NetSpeaker_V2::Start()
{
    DeviceBase::Start();
    if(!Connect())
        return ECCS_ERR_DEV_OFFLINE;

    m_flagReadCmd = true;
    m_ctrlRxThread = new std::thread(&Sound_NetSpeaker_V2::CtrlRxLoop, this);

    return ECCS_SUCCESS;
}

ECCS_Error Sound_NetSpeaker_V2::Stop()
{
    m_flagReadCmd = false;

    if (m_ctrlRxThread)
    {
        m_ctrlRxThread->join();
        delete m_ctrlRxThread;
        m_ctrlRxThread = nullptr;
    }

    Disconnect();
    DeviceBase::Stop();
    return ECCS_SUCCESS;
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
    return m_soundMode.load();
}

// ---------- 播放 ----------

ECCS_Error Sound_NetSpeaker_V2::PlayIndex(int index, bool loop)
{
    if (!SetSoundMode(SoundStatus::Player))
        return ECCS_ERR_DEV_BUSY;

    m_loopPlay = loop;

    char param[64];
    snprintf(param, sizeof(param),
        "\"index\":\"%d\",\"loop\":\"%d\"",
        index, loop ? 1 : 0);

    SendJsonCmd(BuildJson("start_play", param));
    SetState(STATE_WORKING);
    return ECCS_SUCCESS;
}

ECCS_Error Sound_NetSpeaker_V2::StopPlay()
{
    SendJsonCmd(BuildJson("stop_play"));
    SetSoundMode(SoundStatus::Idle);
    SetState(STATE_ONLINE);
    return ECCS_SUCCESS;
}

ECCS_Error Sound_NetSpeaker_V2::Next()
{
    SendJsonCmd(BuildJson("next"));
    return ECCS_SUCCESS;
}

ECCS_Error Sound_NetSpeaker_V2::Prev()
{
    SendJsonCmd(BuildJson("prev"));
    return ECCS_SUCCESS;
}

// ---------- 一键驱散 ----------

ECCS_Error Sound_NetSpeaker_V2::OneKeyPlay(int index)
{
    if (!SetSoundMode(SoundStatus::OneKey))
        return ECCS_ERR_DEV_BUSY;

    char param[64];
    snprintf(param, sizeof(param), "\"index\":\"%d\"", index);
    SendJsonCmd(BuildJson("one_key_play", param));

    SetState(STATE_WORKING);
    return ECCS_SUCCESS;
}

// ---------- 音量 ----------

ECCS_Error Sound_NetSpeaker_V2::SetPlayVolume(u8 vol)
{
    char param[32];
    snprintf(param, sizeof(param), "\"vol\":\"%d\"", vol);
    SendJsonCmd(BuildJson("set_play_vol", param));
    m_playVolume = vol;
    return ECCS_SUCCESS;
}

ECCS_Error Sound_NetSpeaker_V2::GetPlayVolume(u8& vol) const
{
    vol = m_playVolume;
    return ECCS_SUCCESS;
}

ECCS_Error Sound_NetSpeaker_V2::SetCaptureVolume(u8 vol)
{
    char param[32];
    snprintf(param, sizeof(param), "\"vol\":\"%d\"", vol);
    SendJsonCmd(BuildJson("set_cap_vol", param));
    m_captureVolume = vol;
    return ECCS_SUCCESS;
}

// ---------- 音频列表 / 文件 ----------

ECCS_Error Sound_NetSpeaker_V2::GetAudioList(std::vector<SoundFileInfo>& list)
{
    list.clear();
    for (auto& it : m_audioList)
        list.push_back(it);
    return ECCS_SUCCESS;
}

ECCS_Error Sound_NetSpeaker_V2::UploadAudioFile(const str& name, const u8* data, u32 len)
{
    if (!data || len == 0)
        return ECCS_ERR_INVALID_PARAM;

    if (!Connect())
        return ECCS_ERR_DEV_OFFLINE;

    char header[256];
    int seq = ++m_cseq;

    snprintf(header, sizeof(header),
        "{\"command\":\"add_alarm_file\",\"cseq\":\"%d\",\"mp3_len\":\"%u\"}\r\n\r\n",
        seq, len);

    try
    {
        // 1. 先发 JSON 头
        m_ctrlSocket->write((const u8*)header, strlen(header));

        // 2. 再发二进制数据
        m_ctrlSocket->write(data, len);
    }
    catch (...)
    {
        SetState(STATE_ERROR);
        m_ctrlSocket->close();
        return ECCS_ERR_NET_ERROR;
    }

    return ECCS_SUCCESS;
    return ECCS_SUCCESS;
}

ECCS_Error Sound_NetSpeaker_V2::DeleteAudioFile(int)
{
    // 协议后续补
    return ECCS_SUCCESS;
}

// ---------- 实时音频 ----------

ECCS_Error Sound_NetSpeaker_V2::PushAudio(const u8* data, u32 len)
{
    if (m_soundMode != SoundStatus::MicBroadcast)
        return ECCS_ERR_DEV_BUSY;

    if (m_audioTxBuf) {
        m_audioTxBuf->Write(data, len);
    }
    else {
        return ECCS_ERR_MALLOC;
    }

    return ECCS_SUCCESS;
}

// =================================================
// DeviceBase 状态机钩子
// =================================================

void Sound_NetSpeaker_V2::OnCustomEvent(Event_Ptr& e)
{

}

void Sound_NetSpeaker_V2::OnStateEnter(DevState state)
{
    if (state == STATE_ONLINE)
    {
        // 设备信息 & 配置同步
        QueryDeviceInfo();
        QueryVolume();
        QueryAudioList();

        // 心跳
        m_keepHeartbeat = true;
        m_heartbeatThread = new std::thread(&Sound_NetSpeaker_V2::HeartbeatLoop, this);

        // UDP 音频
        m_audioTxBuf = new RingBuffer(1024 * 100);
        m_keepAudioTx = true;
        m_audioTxThread = new std::thread(&Sound_NetSpeaker_V2::AudioTxLoop, this);

        m_keepAudioRx = true;
        m_audioRxThread = new std::thread(&Sound_NetSpeaker_V2::AudioRxLoop, this);
    }
}

void Sound_NetSpeaker_V2::OnStateExit(DevState state)
{
    if (state == STATE_ONLINE)
    {
        m_keepHeartbeat = false;
        m_keepAudioTx = false;
        m_keepAudioRx = false;
        m_flagReadCmd = false;

        if (m_heartbeatThread)
        {
            m_heartbeatThread->join();
            delete m_heartbeatThread;
            m_heartbeatThread = nullptr;
        }

        if (m_audioTxThread)
        {
            m_audioTxThread->join();
            delete m_audioTxThread;
            m_audioTxThread = nullptr;
        }

        if (m_audioRxThread)
        {
            m_audioRxThread->join();
            delete m_audioRxThread;
            m_audioRxThread = nullptr;
        }

        if (m_ctrlRxThread)
        {
            m_ctrlRxThread->join();
            delete m_ctrlRxThread;
            m_ctrlRxThread = nullptr;
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

void Sound_NetSpeaker_V2::CtrlRxLoop()
{
    u8 buf[1024];

    while (m_flagReadCmd) 
    {
        if (!m_ctrlSocket || !m_ctrlSocket->isOpen())
        {
            msleep(100);
            continue;
        }

        int len = 0;
        try {
            len = m_ctrlSocket->read(buf, sizeof(buf));
        }
        catch (...) {
            SetState(STATE_ERROR);
            break;
        }

        if (len <= 0)
        {
            msleep(10);
            continue;
        }

        m_ctrlRxCache.append((char*)buf, len);

        // JSON 以 \r\n\r\n 结尾
        size_t pos;
        while ((pos = m_ctrlRxCache.find("\r\n\r\n")) != std::string::npos)
        {
            std::string one = m_ctrlRxCache.substr(0, pos);
            m_ctrlRxCache.erase(0, pos + 4);
            HandleJsonReply(one);
        }
    }
}

static bool ExtractStr(const str& src, const str& key, str& out)
{
    size_t p = src.find("\"" + key + "\"");
    if (p == str::npos) return false;

    p = src.find(':', p);
    if (p == str::npos) return false;

    size_t q1 = src.find('"', p);
    size_t q2 = src.find('"', q1 + 1);
    if (q1 == str::npos || q2 == str::npos) return false;

    out = src.substr(q1 + 1, q2 - q1 - 1);
    return true;
}

static bool ExtractInt(const str& src, const char* key, int& out)
{
    str k = "\"" + str(key) + "\"";
    size_t p = src.find(k);
    if (p == str::npos) return false;

    p = src.find(':', p);
    out = atoi(src.c_str() + p + 1);
    return true;
}

void Sound_NetSpeaker_V2::HandleJsonReply(const str& json)
{
    LOG_DEBUG("[NetSpeaker] RX: %s", json.c_str());

    str cmd;
    if (!ExtractStr(json, "command", cmd))
        return;

    // ---------- 状态 ----------
    if (cmd == "post_status")
    {
        str model;
        if (ExtractStr(json, "model", model))
        {
            if (model == "idle")         m_soundMode = SoundStatus::Idle;
            else if (model == "player")  m_soundMode = SoundStatus::Player;
            else if (model == "one_key") m_soundMode = SoundStatus::OneKey;
            else if (model == "mic_broadcast") m_soundMode = SoundStatus::MicBroadcast;
        }
    }

    // ---------- 音量 ----------
    else if (cmd == "post_vol")
    {
        int v;
        if (ExtractInt(json, "play_vol", v))
            m_playVolume = (u8)v;
        if (ExtractInt(json, "cap_vol", v))
            m_captureVolume = (u8)v;
    }

    // ---------- 播放列表 ----------
    else if (cmd == "get_play_list")
    {
        m_audioList.clear();

        size_t arrBeg = json.find('[');
        size_t arrEnd = json.find(']', arrBeg);
        if (arrBeg == str::npos || arrEnd == str::npos)
            return;

        str arr = json.substr(arrBeg + 1, arrEnd - arrBeg - 1);
        size_t p = 0;

        while ((p = arr.find('{', p)) != str::npos)
        {
            size_t e = arr.find('}', p);
            if (e == str::npos) break;

            str obj = arr.substr(p + 1, e - p - 1);

            SoundFileInfo info{};
            ExtractInt(obj, "index", info.index);
            ExtractInt(obj, "duration", (int&)info.duration);
            ExtractStr(obj, "name", info.name);

            const str prefix = "/xmedia/mp3/";
            if (info.name.find(prefix) == 0)
                info.name = info.name.substr(prefix.size());

            m_audioList.push_back(info);
            p = e + 1;
        }
    }

    // ---------- 上传 MP3 / 报警音频 ACK ----------
    else if (cmd == "mp3_stream_play" || cmd == "add_alarm_file")
    {
        // 这里只需要确认成功即可，协议未定义更多字段
        LOG_INFO("[NetSpeaker] upload ACK ok");
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

void Sound_NetSpeaker_V2::AudioRxLoop()
{
    UdpSocket udp(9889);
    try { udp.open(); }
    catch (...) { return; }

    u8 buf[1024];
    while (m_keepAudioRx)
    {
        int len = udp.read(buf, sizeof(buf));
        if (len > 0)
            NotifyAudioCapture(buf, len);
    }
}

// =================================================
// 设备信息 / 配置同步
// =================================================

bool Sound_NetSpeaker_V2::QueryDeviceInfo()
{
    SendJsonCmd(BuildJson("get_device_info"));
    return true;
}

bool Sound_NetSpeaker_V2::QueryVolume()
{
    SendJsonCmd(BuildJson("get_volume"));
    return true;
}

bool Sound_NetSpeaker_V2::QueryAudioList()
{
    SendJsonCmd(BuildJson("get_audio_list"));
    return true;
}

bool Sound_NetSpeaker_V2::SyncConfigFromDevice()
{
    QueryVolume();
    QueryAudioList();
    return true;
}

bool Sound_NetSpeaker_V2::SyncConfigToDevice()
{
    SetPlayVolume(m_playVolume);
    SetCaptureVolume(m_captureVolume);
    return true;
}

ECCS_END
