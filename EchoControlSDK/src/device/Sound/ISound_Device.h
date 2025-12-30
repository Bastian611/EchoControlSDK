#pragma once
#include "../DeviceBase.h"
#include "device/DeviceDataTypes.h"
#include <functional>
#include <vector>

ECCS_BEGIN

class ISound_Device : public DeviceBase
{
public:
    // =================================================
    // 模式控制
    // =================================================
    virtual bool SetSoundMode(SoundStatus mode)
    {
        LOG_WARNING("[Slot %d] Device does not support SetSoundMode()", m_slotID);
        return false;
    }

    virtual SoundStatus GetSoundMode() const
    {
        return SoundStatus::Idle;
    }

    // =================================================
    // 播放控制
    // =================================================
    virtual bool PlayIndex(int index, bool loop = false)
    {
        LOG_WARNING("[Slot %d] Device does not support PlayIndex()", m_slotID);
        return false;
    }

    virtual bool StopPlay()
    {
        LOG_WARNING("[Slot %d] Device does not support StopPlay()", m_slotID);
        return false;
    }

    virtual bool Next()
    {
        LOG_WARNING("[Slot %d] Device does not support Next()", m_slotID);
        return false;
    }

    virtual bool Prev()
    {
        LOG_WARNING("[Slot %d] Device does not support Prev()", m_slotID);
        return false;
    }

    // =================================================
    // 一键驱散
    // =================================================
    virtual bool OneKeyPlay(int index)
    {
        LOG_WARNING("[Slot %d] Device does not support OneKeyPlay()", m_slotID);
        return false;
    }

    // =================================================
    // 音量控制（Config 同步）
    // =================================================
    virtual bool SetPlayVolume(u8 vol)
    {
        LOG_WARNING("[Slot %d] Device does not support SetPlayVolume()", m_slotID);
        return false;
    }

    virtual bool GetPlayVolume(u8& vol) const
    {
        return false;
    }

    virtual bool SetCaptureVolume(u8 vol)
    {
        LOG_WARNING("[Slot %d] Device does not support SetCaptureVolume()", m_slotID);
        return false;
    }

    virtual bool GetCaptureVolume(u8& vol) const
    {
        return false;
    }

    // =================================================
    // 音频列表 / 文件管理
    // =================================================
    virtual bool GetAudioList(std::vector<str>& list)
    {
        LOG_WARNING("[Slot %d] Device does not support GetAudioList()", m_slotID);
        return false;
    }

    virtual bool UploadAudioFile(const str& name, const u8* data, u32 len)
    {
        LOG_WARNING("[Slot %d] Device does not support UploadAudioFile()", m_slotID);
        return false;
    }

    virtual bool DeleteAudioFile(int index)
    {
        LOG_WARNING("[Slot %d] Device does not support DeleteAudioFile()", m_slotID);
        return false;
    }

    // =================================================
    // 实时音频（全双工）
    // =================================================
    virtual bool PushAudio(const u8* data, u32 len)
    {
        LOG_WARNING("[Slot %d] Device does not support PushAudio()", m_slotID);
        return false;
    }

    // =================================================
    // 回调：下行音频（设备 → SDK → 用户）
    // =================================================
    using AudioCallback = std::function<void(const u8*, u32)>;

    void SetCaptureCallback(AudioCallback cb)
    {
        m_audioCb = cb;
    }

    // =================================================
    // 回调：播放状态
    // =================================================
    using PlayStateCallback = std::function<void(SoundPlayState)>;

    void SetPlayStateCallback(PlayStateCallback cb)
    {
        m_playStateCb = cb;
    }

protected:
    // =================================================
    // 派生类辅助通知接口
    // =================================================
    void NotifyAudioCapture(const u8* data, u32 len)
    {
        if (m_audioCb)
            m_audioCb(data, len);
    }

    void NotifyPlayState(SoundPlayState state)
    {
        if (m_playStateCb)
            m_playStateCb(state);
    }

    bool IsSoundOnline() const
    {
        return IsOnline() &&
            (GetState() == STATE_ONLINE || GetState() == STATE_WORKING);
    }

protected:
    AudioCallback     m_audioCb;
    PlayStateCallback m_playStateCb;
};

ECCS_END
