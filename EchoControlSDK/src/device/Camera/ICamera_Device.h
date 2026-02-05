#pragma once
#include "../DeviceBase.h"

ECCS_BEGIN

// 视频帧类型定义
enum class ECCS_VideoType : u8 {
    Video_I = 0,   // I帧
    Video_P = 1,   // P帧
    Audio = 2,     // 音频
    Header = 3     // 解码参数
};

class ICamera_Device : public DeviceBase
{
public:
    // --- 视频流控制 ---
    virtual ECCS_Error StartStream(int channel = 0, int streamType = 0) = 0;
    virtual ECCS_Error StopStream() = 0;

    // --- 抓拍与录像 ---
    virtual ECCS_Error Snapshot(const str& savePath) = 0;

    // --- 回调设置 ---
    using VideoDataCallback = std::function<void(const u8* data, u32 len, ECCS_VideoType type)>;
    void SetVideoCallback(VideoDataCallback cb) { m_videoCb = cb; }

protected:
    VideoDataCallback m_videoCb;
};

ECCS_END