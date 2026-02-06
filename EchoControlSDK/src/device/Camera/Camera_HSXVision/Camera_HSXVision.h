#pragma once
#include "../ICamera_Device.h"
#include "net/TCPSocket.h"
#include <mutex>

#ifdef WIN32
#include "../../../../../3rd/hsx_vision/win/include/AjNetSdkDll.h"
#include "../../../../../3rd/hsx_vision/win/include/AjPlayer.h"
#else
#include "hsx/NetSDKDLL.h" 
#endif

ECCS_BEGIN

class Camera_HSXVision : public ICamera_Device
{
public:
    using Self = Camera_HSXVision;
    static const u32 ID = MAKE_DEV_OID(did::DEVICE_CAMERA, did::CAMERA_HSXVISION);
    FACTORY_ID_CHILD_WITH_SPEC_NAME(u32, ID, DeviceBase, Self, "HSX-Vision")

    Camera_HSXVision();
    virtual ~Camera_HSXVision();

    // --- 生命周期 ---
    virtual ECCS_Error Init(int slotID, const std::map<str, str>& config) override;
    virtual ECCS_Error Start() override;
    virtual ECCS_Error Stop() override;
    virtual bool Reconnect() override;

    // --- 业务接口实现 ---
    virtual ECCS_Error StartStream(VideoOutMode mode) override;
    virtual ECCS_Error StopStream() override;
    virtual ECCS_Error Snapshot(const char* savePath) override;
    virtual ECCS_Error Focus(CameraFocusAction action) override;
    virtual ECCS_Error GetVideoParams(VideoParams& outParams) override;

protected:
    // 厂商 SDK 回调适配
    static void CALLBACK GlobalStatusCallBack(long lUser, long nStateCode, char* pResponse, void* pUser);
    static void CALLBACK GlobalRealDataCallBack(long lRealHandle, DWORD dwDataType, BYTE* pBuffer, DWORD dwBufSize, LPFRAME_EXTDATA pExtData);

#ifdef WIN32
    // Windows 专用解码回调
    static int CALLBACK GlobalDecodeCallBack(long nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, void* pUser, long nReserved2);
#endif

private:
    bool Login();
    void Logout();

private:
    // --- 厂商句柄 ---
    long m_lUserID{ -1 };
    long m_lRealHandle{ -1 };
    long m_lPlayerPort{ -1 }; // DllPlayer 端口

    // --- 配置信息 ---
    str m_ip;
    int m_port;
    str m_user;
    str m_pass;

    // --- 静态资源管理 (厂商 SDK 生命周期) ---
    static std::mutex s_sdkMutex;
    static int s_instanceCount;
};

ECCS_END