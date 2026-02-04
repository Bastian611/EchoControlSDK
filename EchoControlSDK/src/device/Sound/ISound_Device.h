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
    virtual ECCS_Error PlayIndex(int index, bool loop = false)
    {
        LOG_WARNING("[Slot %d] Device does not support PlayIndex()", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
    }

    virtual ECCS_Error StopPlay()
    {
        LOG_WARNING("[Slot %d] Device does not support StopPlay()", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
    }

    virtual ECCS_Error Next()
    {
        LOG_WARNING("[Slot %d] Device does not support Next()", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
    }

    virtual ECCS_Error Prev()
    {
        LOG_WARNING("[Slot %d] Device does not support Prev()", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
    }

    virtual ECCS_Error MicSwitch(bool isOpen)
    {
        LOG_WARNING("[Slot %d] Device does not support", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
    }

    // =================================================
    // 一键驱散
    // =================================================
    virtual ECCS_Error OneKeyPlay(int index)
    {
        LOG_WARNING("[Slot %d] Device does not support OneKeyPlay()", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
    }

    // =================================================
    // 音量控制（Config 同步）
    // =================================================
    virtual ECCS_Error SetPlayVolume(u8 vol)
    {
        LOG_WARNING("[Slot %d] Device does not support SetPlayVolume()", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
    }

    virtual ECCS_Error GetPlayVolume(u8& vol) const
    {
        return ECCS_ERR_NOT_SUPPORTED;
    }

    virtual ECCS_Error SetCaptureVolume(u8 vol)
    {
        LOG_WARNING("[Slot %d] Device does not support SetCaptureVolume()", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
    }

    virtual ECCS_Error GetCaptureVolume(u8& vol) const
    {
        return ECCS_ERR_NOT_SUPPORTED;
    }

    // =================================================
    // 音频列表 / 文件管理
    // =================================================
    virtual ECCS_Error GetAudioList(std::vector<SoundFileInfo>& list)
    {
        LOG_WARNING("[Slot %d] Device does not support GetAudioList()", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
    }

    virtual ECCS_Error UploadAudioFile(const str& name, const u8* data, u32 len)
    {
        LOG_WARNING("[Slot %d] Device does not support UploadAudioFile()", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
    }

    virtual ECCS_Error DeleteAudioFile(int index)
    {
        LOG_WARNING("[Slot %d] Device does not support DeleteAudioFile()", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
    }

    // =================================================
    // 实时音频（全双工）
    // =================================================
    virtual ECCS_Error PushAudio(const u8* data, u32 len)
    {
        LOG_WARNING("[Slot %d] Device does not support PushAudio()", m_slotID);
        return ECCS_ERR_NOT_SUPPORTED;
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
