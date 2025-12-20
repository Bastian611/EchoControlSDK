#include "Sound_NetSpeaker_V2.h"
#include "debug/Exceptions.h"
#include "debug/Logger.h"
#include "time/time_utils.h"
#include <chrono>

ECCS_BEGIN

Sound_NetSpeaker_V2::Sound_NetSpeaker_V2()
    : m_cseq(0), m_heartbeatThread(nullptr), m_keepHeartbeat(false)
{

}

Sound_NetSpeaker_V2::~Sound_NetSpeaker_V2() 
{
    Stop(); // 确保线程停止
}

bool Sound_NetSpeaker_V2::Init(int slotID, const std::map<str, str>& config) 
{
    if (!DeviceBase::Init(slotID, config)) return false;

    m_ip = GetPropValue<str>("IP");
    m_port = GetPropValue<int>("Port");

    // 强声设备默认端口通常是 9527，如果配置没填，可以给个默认值
    if (m_port == 0) m_port = 9527;

    return true;
}

bool Sound_NetSpeaker_V2::Start() 
{
    if (!DeviceBase::Start()) return false;

    // 启动心跳线程
    m_keepHeartbeat = true;
    m_heartbeatThread = new std::thread(&Sound_NetSpeaker_V2::HeartbeatLoop, this);

    return true;
}

void Sound_NetSpeaker_V2::Stop() 
{
    // 停止心跳线程
    m_keepHeartbeat = false;
    if (m_heartbeatThread) {
        if (m_heartbeatThread->joinable()) m_heartbeatThread->join();
        delete m_heartbeatThread;
        m_heartbeatThread = nullptr;
    }

    // 关闭 Socket
    if (m_socket) m_socket->close();

    // 停止基类工作线程
    DeviceBase::Stop();
}

// --- 业务逻辑 ---

void Sound_NetSpeaker_V2::PlayFile(const char* filename, bool loop) 
{
    // 协议：{"command":"start_play","cseq":"x", "index":"filename"}
    // 注意：如果 filename 是路径，协议可能不同，这里按通用处理
    char param[256];

    // 假设 filename 是索引号或者路径
    // 如果是路径，通常用 one_key 模式，这里简化为 start_play
    snprintf(param, sizeof(param), "\"index\":\"%s\"", filename);

    SendJsonCmd(BuildJson("start_play", param));
}

void Sound_NetSpeaker_V2::StopPlay() 
{
    SendJsonCmd(BuildJson("stop_play"));
}

void Sound_NetSpeaker_V2::TTSPlay(const char* text) 
{
    // 协议：start_tts_play, txt=...
    char param[512];
    snprintf(param, sizeof(param), "\"txt\":\"%s\",\"play_vol\":\"80\"", text);
    SendJsonCmd(BuildJson("start_tts_play", param));
}

void Sound_NetSpeaker_V2::SetMic(bool isOpen) 
{
    // 喊话模式需要切换 model
    if (isOpen) {
        // 切换到 mic_broadcast 模式
        SendJsonCmd(BuildJson("model_change", "\"model\":\"mic_broadcast\""));
        // 实际喊话还需要建立 UDP 传输音频流，这部分较复杂，
        // 这里先只发控制指令，UDP流通常由上层或者单独的模块处理
    }
    else {
        // 切回空闲
        SendJsonCmd(BuildJson("model_change", "\"model\":\"idle\""));
    }
}

void Sound_NetSpeaker_V2::SetVolume(u8 vol)
{

}

void Sound_NetSpeaker_V2::GetVolume(u8 vol_play, u8 vol_cap)
{

}

// --- 内部辅助 ---

str Sound_NetSpeaker_V2::BuildJson(const char* cmd, const char* params) 
{
    char buf[1024];
    m_cseq++;

    if (params && strlen(params) > 0) {
        snprintf(buf, sizeof(buf), "{\"command\":\"%s\",\"cseq\":\"%d\",%s}\r\n\r\n",
            cmd, m_cseq, params);
    }
    else {
        snprintf(buf, sizeof(buf), "{\"command\":\"%s\",\"cseq\":\"%d\"}\r\n\r\n",
            cmd, m_cseq);
    }
    return str(buf);
}

void Sound_NetSpeaker_V2::SendJsonCmd(const str& json) 
{
    if (!Connect()) return;

    try {
        // TCP 发送
        m_socket->write((const u8*)json.c_str(), json.length());
        // LOG_DEBUG("[Sound] Sent: %s", json.c_str()); 
    }
    catch (std::exception& e) {
        LOG_ERROR("[Slot %d] Sound Send Error: %s", m_slotID, e.what());
        SetState(STATE_ERROR);
        m_socket->close();
    }
}

bool Sound_NetSpeaker_V2::Connect() 
{
    if (IsOnline() && m_socket && m_socket->isOpen()) return true;

    try {
        SetState(STATE_CONNECTING);
        m_socket = std::make_shared<TcpSocket>(m_ip, m_port);
        m_socket->setConnTimeout(1000); // 1秒超时
        m_socket->open();
        SetState(STATE_ONLINE);
        return true;
    }
    catch (std::exception&) {
        SetState(STATE_OFFLINE);
        return false;
    }
}

// 独立的线程发送心跳
void Sound_NetSpeaker_V2::HeartbeatLoop() 
{
    int counter = 0;
    while (m_keepHeartbeat) {
        msleep(100);
        counter++;

        if (counter >= 300) { // 30秒
            counter = 0;
            if (IsOnline()) {
                postEvent(new Event(EVENT_HEARTBEAT));
            }
        }
    }
}

// 在主线程中处理事件
void Sound_NetSpeaker_V2::OnCustomEvent(Event_Ptr& e)
{
    // 判断是否是心跳事件
    if (e->eId() == EVENT_HEARTBEAT) 
    {
        SendJsonCmd(BuildJson("online"));
        // LOG_DEBUG("Heartbeat sent via main thread.");
    }
}

ECCS_END