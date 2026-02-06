#include "Camera_HSXVision.h"
#include "debug/Logger.h"
#include "protocol/Packet_Def.h"
#include <mutex>

ECCS_BEGIN

// 静态成员初始化
std::mutex Camera_HSXVision::s_sdkMutex;
int Camera_HSXVision::s_instanceCount = 0;

Camera_HSXVision::Camera_HSXVision()
{
}

Camera_HSXVision::~Camera_HSXVision()
{
    Stop();
}

// =====================================================================
// 生命周期管理
// =====================================================================

ECCS_Error Camera_HSXVision::Init(int slotID, const std::map<str, str>& config)
{
    if (DeviceBase::Init(slotID, config) != ECCS_SUCCESS) return ECCS_ERR_NOT_INIT;

    m_ip = GetPropValue<str>("IP");
    m_port = GetPropValue<int>("Port");
    m_user = GetPropValue<str>("User");
    m_pass = GetPropValue<str>("Pass");

    // 1. 厂商 SDK 全局初始化 (单例引用计数)
    {
        std::lock_guard<std::mutex> lock(s_sdkMutex);
        if (s_instanceCount == 0) {
#ifdef WIN32
            AJ_NETSDK_IPC_Init();
            AJ_PLAYER_Init();
#else
            // Linux 版对应接口
            IP_NET_DVR_Init();
#endif
            LOG_INFO("HSX Vendor SDK Initialized.");
        }
        s_instanceCount++;
    }

    return ECCS_SUCCESS;
}

ECCS_Error Camera_HSXVision::Start()
{
    DeviceBase::Start();
    if (!Login()) return ECCS_ERR_DEV_OFFLINE;

    // 注册状态回调用于接收 AI 告警
#ifdef WIN32
    AJ_NETSDK_IPC_SetStatusEventCallBack(GlobalStatusCallBack, this);
#else
    IP_NET_DVR_SetStatusEventCallBack(GlobalStatusCallBack, this);
#endif

    return ECCS_SUCCESS;
}

ECCS_Error Camera_HSXVision::Stop()
{
    StopStream();
    Logout();

    {
        std::lock_guard<std::mutex> lock(s_sdkMutex);
        s_instanceCount--;
        if (s_instanceCount == 0) {
#ifdef WIN32
            AJ_NETSDK_IPC_Cleanup();
            AJ_PLAYER_ReleaseAll();
#else
            IP_NET_DVR_Cleanup();
#endif
        }
    }
    return DeviceBase::Stop();
}

bool Camera_HSXVision::Reconnect()
{
    Logout();
    return Login();
}

// =====================================================================
// 动作控制 (CONTROL)
// =====================================================================

ECCS_Error Camera_HSXVision::StartStream(VideoOutMode mode)
{
    if (m_lUserID <= 0) return ECCS_ERR_DEV_OFFLINE;
    m_outMode = mode;

    // 1. 如果需要解码 (YUV/RGB)，初始化解码器 (仅 Windows 支持)
    if (m_outMode != VideoOutMode::RawData) {
#ifdef WIN32
        m_lPlayerPort = AJ_PLAYER_CreatePlayer(NULL); // 传入 NULL 表示仅解码不显示
        if (m_lPlayerPort > 0) {
            AJ_PLAYER_SetDecCallBack(m_lPlayerPort, GlobalDecodeCallBack, this);
            AJ_PLAYER_SetPlayOn(m_lPlayerPort, 1);
        }
#else
        LOG_ERROR("Decoding mode is not supported on Linux yet.");
        return ECCS_ERR_NOT_SUPPORTED;
#endif
    }

    // 2. 开启拉流
    AJ_NETSDK_IPC_CLIENTINFO info = { 0 };
    info.lChannel = 0; // 默认通道 0
    info.lLinkMode = 1; // TCP 模式

#ifdef WIN32
    m_lRealHandle = AJ_NETSDK_IPC_RealPlay(m_lUserID, &info, GlobalRealDataCallBack, this, FALSE);
#else
    m_lRealHandle = IP_NET_DVR_RealPlay(m_lUserID, &info, GlobalRealDataCallBack, this, FALSE);
#endif

    if (m_lRealHandle > 0) {
        SetState(STATE_WORKING);
        return ECCS_SUCCESS;
    }
    return ECCS_ERR_DEV_SEND_FAILED;
}

ECCS_Error Camera_HSXVision::StopStream()
{
    if (m_lRealHandle > 0) {
#ifdef WIN32
        AJ_NETSDK_IPC_StopRealPlay(m_lRealHandle);
        if (m_lPlayerPort > 0) {
            AJ_PLAYER_StopPlay(m_lPlayerPort);
            AJ_PLAYER_DeletePlayer(m_lPlayerPort);
            m_lPlayerPort = -1;
        }
#else
        IP_NET_DVR_StopRealPlay(m_lRealHandle);
#endif
        m_lRealHandle = -1;
    }
    SetState(STATE_ONLINE);
    return ECCS_SUCCESS;
}

ECCS_Error Camera_HSXVision::Focus(CameraFocusAction action)
{
    // 映射调焦指令
    u32 cmd = (action == CameraFocusAction::FocusIn) ? FOCUS_NEAR : FOCUS_FAR;
#ifdef WIN32
    return AJ_NETSDK_IPC_PTZControl(m_lUserID, cmd, 5, 5) == 0 ? ECCS_SUCCESS : ECCS_ERR_DEV_SEND_FAILED;
#else
    return IP_NET_DVR_PTZControl(m_lUserID, cmd, 5, 5) == 0 ? ECCS_SUCCESS : ECCS_ERR_DEV_SEND_FAILED;
#endif
}

ECCS_Error Camera_HSXVision::Snapshot(const char* savePath)
{
    if (m_lUserID <= 0) return ECCS_ERR_DEV_OFFLINE;
#ifdef WIN32
    // 使用文件抓图接口
    int ret = AJ_NETSDK_IPC_GetFileByName(m_lUserID, RECORD_FILE, (char*)"snap.jpg", (char*)savePath);
    return ret == 0 ? ECCS_SUCCESS : ECCS_ERR_FAILED;
#else
    return ECCS_ERR_NOT_SUPPORTED;
#endif
}

// =====================================================================
// 回调处理 (路由转换)
// =====================================================================

// 1. 处理原始流回调 (来自网络库)
void CALLBACK Camera_HSXVision::GlobalRealDataCallBack(LONG lRealHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, LPFRAME_EXTDATA pExtData)
{
    Camera_HSXVision* pThis = (Camera_HSXVision*)pExtData->pUserData;
    if (!pThis) return;

    // dwDataType: 0:视频, 1:音频, 2:解码参数
    if (pThis->m_outMode == VideoOutMode::RawData) {
        if (pThis->m_videoCb) {
            // 透传 H.264/H.265 裸流给用户
            pThis->m_videoCb(pBuffer, dwBufSize, (u8)dwDataType, false);
        }
    }
    else {
        // 解码模式：将原始数据送入播放库进行解码
#ifdef WIN32
        if (pThis->m_lPlayerPort > 0) {
            if (dwDataType == 2) {
                // 收到参数包，配置解码器
                AJ_PLAYER_SetupDecoder(pThis->m_lPlayerPort, 1, pBuffer, dwBufSize);
            }
            else {
                // 送入解码帧
                AJ_PLAYER_DecodeFrame(pThis->m_lPlayerPort, (dwDataType == 0), pBuffer, dwBufSize, NULL, 0);
            }
        }
#endif
    }
}

// 2. 处理解码后数据回调 (来自播放库)
#ifdef WIN32
int CALLBACK Camera_HSXVision::GlobalDecodeCallBack(long nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, void* pUser, long nReserved2)
{
    Camera_HSXVision* pThis = (Camera_HSXVision*)pUser;
    if (pThis && pThis->m_videoCb) {
        // pBuf 此时是解码后的 YUV 数据
        // pFrameInfo->bIsVideo 为 1 表示视频
        pThis->m_videoCb((u8*)pBuf, (u32)nSize, (u8)(pFrameInfo->bIsVideo ? 0 : 2), true);
    }
    return 0;
}
#endif

// 3. 处理状态/AI 告警回调 (来自网络库)
void CALLBACK Camera_HSXVision::GlobalStatusCallBack(long lUser, long nStateCode, char* pResponse, void* pUser)
{
    Camera_HSXVision* pThis = (Camera_HSXVision*)pUser;
    if (!pThis) return;

    // nStateCode == 14 为移动侦测 (EVENT_PTZALARM 参考文档)
    if (nStateCode == EVENT_PTZALARM || nStateCode == 14) {
        CameraAlarm alarm;
        alarm.alarmType = (u32)nStateCode;
        strncpy(alarm.description, pResponse ? pResponse : "AI Motion Detected", sizeof(alarm.description) - 1);

        if (pThis->m_alarmCb) pThis->m_alarmCb(alarm);
    }

    // 更新通用健康状态
    if (nStateCode == EVENT_LOGINOK) pThis->SetState(STATE_ONLINE);
    if (nStateCode == EVENT_CONNECTFAILED) pThis->SetState(STATE_OFFLINE);
}

// =====================================================================
// 内部辅助
// =====================================================================

bool Camera_HSXVision::Login()
{
#ifdef WIN32
    m_lUserID = AJ_NETSDK_IPC_Login((char*)m_ip.c_str(), (WORD)m_port, (char*)m_user.c_str(), (char*)m_pass.c_str(), NULL);
#else
    m_lUserID = IP_NET_DVR_Login((char*)m_ip.c_str(), (WORD)m_port, (char*)m_user.c_str(), (char*)m_pass.c_str(), NULL);
#endif
    return m_lUserID > 0;
}

void Camera_HSXVision::Logout()
{
    if (m_lUserID > 0) {
#ifdef WIN32
        AJ_NETSDK_IPC_Logout(m_lUserID);
#else
        IP_NET_DVR_Logout(m_lUserID);
#endif
        m_lUserID = -1;
    }
}

ECCS_Error Camera_HSXVision::GetVideoParams(VideoParams& outParams) {
    // 华视芯获取参数通常需要异步解析 XML，此处可调用同步等待逻辑或返回缓存
    return ECCS_ERR_NOT_SUPPORTED;
}

ECCS_END