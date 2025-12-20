#pragma once
#include "../DeviceBase.h"

ECCS_BEGIN

class ISound_Device : public DeviceBase
{
public:
    // === ´¿Ðé½Ó¿Ú ===
    virtual void PlayFile(const char* filename, bool loop) = 0;
    virtual void StopPlay() = 0;
    virtual void TTSPlay(const char* text) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support TTS.",
            m_slotID, GetProperty("Model").c_str());
    }
    virtual void SetMic(bool isOpen) {
        LOG_WARNING("[Slot %d] Device (Model: %s) does not support Mic.",
            m_slotID, GetProperty("Model").c_str());
    }
};

ECCS_END