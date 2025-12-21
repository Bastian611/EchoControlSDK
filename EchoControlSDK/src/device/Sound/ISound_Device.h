#pragma once
#include "../DeviceBase.h"

ECCS_BEGIN

class ISound_Device : public DeviceBase
{
public:
    // === 纯虚接口 ===
    virtual void PlayFile(const char* filename, bool loop) = 0;
    virtual void StopPlay() = 0;
    virtual void SetVolume(u8 vol) = 0;
    virtual void GetVolume(u8 vol_play, u8 vol_cap) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support GetVolume.",
            m_slotID, GetProperty("Model").c_str());
    }
    virtual void TTSPlay(const char* text) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support TTS.",
            m_slotID, GetProperty("Model").c_str());
    }
    virtual void SetMic(bool isOpen) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support Mic.",
            m_slotID, GetProperty("Model").c_str());
    }
    // 推送音频数据接口
    virtual void PushAudio(const u8* data, u32 len) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support Mic.",
            m_slotID, GetProperty("Model").c_str());
    }
};

ECCS_END