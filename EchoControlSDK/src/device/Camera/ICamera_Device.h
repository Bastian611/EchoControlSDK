#pragma once
#include "../DeviceBase.h"
#include "../DeviceDataTypes.h"

ECCS_BEGIN

// 视频帧类型定义
enum class VideoType : u8 {
    Video_I = 0,   // I帧
    Video_P = 1,   // P帧
    Audio = 2,     // 音频
    Header = 3     // 解码参数
};

class ICamera_Device : public DeviceBase
{
public:
    // --- CONTROL ---
    virtual ECCS_Error StartStream(VideoOutMode mode) = 0; // mode: Raw/YUV/RGB
    virtual ECCS_Error StopStream() = 0;
    virtual ECCS_Error Snapshot(const char* savePath) = 0;
    virtual ECCS_Error Focus(CameraFocusAction action) = 0; // 调焦控制

    // --- QUERY (主动查询) ---
    virtual ECCS_Error GetVideoParams(VideoParams& outParams) = 0;

    // --- 回调设置 (SDK内部路由使用) ---
    using VideoCallback = std::function<void(const u8* data, u32 len, u8 type, bool isDecoded)>;
    using AlarmCallback = std::function<void(const CameraAlarm&)>;
    using StatusCallback = std::function<void(const CameraStatus&)>;

    void SetVideoCallback(VideoCallback cb) { m_videoCb = cb; }
    void SetAlarmCallback(AlarmCallback cb) { m_alarmCb = cb; }
    void SetCamStatusCallback(StatusCallback cb) { m_camStatusCb = cb; }

protected:
    VideoCallback  m_videoCb;
    AlarmCallback  m_alarmCb;
    StatusCallback m_camStatusCb;
};

ECCS_END