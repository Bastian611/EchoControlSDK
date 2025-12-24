#pragma once
#include "../DeviceBase.h"
#include "device/DeviceDataTypes.h"

ECCS_BEGIN

class ISound_Device : public DeviceBase
{
public:
    // =================================================
    // 模式控制
    // =================================================
    virtual bool SetSoundMode(SoundStatus mode) = 0;
    virtual SoundStatus GetSoundMode() const = 0;

    // =================================================
    // 播放控制
    // =================================================
    virtual bool PlayIndex(int index, bool loop = false) = 0;
    virtual bool StopPlay() = 0;
    virtual bool Next() = 0;
    virtual bool Prev() = 0;
    virtual bool SetSingleLoop(bool enable) = 0;

    // =================================================
    // 一键驱散
    // =================================================
    virtual bool OneKeyPlay(int index) = 0;

    // =================================================
    // 音量控制（Config 同步）
    // =================================================
    virtual bool SetPlayVolume(u8 vol) = 0;
    virtual bool GetPlayVolume(u8& vol) const = 0;

    virtual bool SetCaptureVolume(u8 vol) = 0;
    virtual bool GetCaptureVolume(u8& vol) const = 0;

    // =================================================
    // 音频列表 / 文件管理
    // =================================================
    virtual bool GetAudioList(std::vector<SoundFileInfo>& list) = 0;
    virtual bool UploadAudioFile(const char* name, const u8* data, u32 len) = 0;
    virtual bool DeleteAudioFile(int index) = 0;

    // =================================================
    // 实时音频（全双工）
    // =================================================
    virtual bool PushAudio(const u8* data, u32 len) = 0;

    using AudioCallback = std::function<void(const u8*, u32)>;
    void SetCaptureCallback(AudioCallback cb) {
        m_audioCb = cb;
    }

    // =================================================
    // 播放状态回调
    // =================================================
    using PlayStateCallback = std::function<void(SoundPlayState)>;
    void SetPlayStateCallback(PlayStateCallback cb) {
        m_playStateCb = cb;
    }

protected:
    bool IsSoundOnline() const {
        return IsOnline() &&
            (GetState() == STATE_ONLINE || GetState() == STATE_WORKING);
    }

protected:
    AudioCallback      m_audioCb;
    PlayStateCallback  m_playStateCb;
};

ECCS_END