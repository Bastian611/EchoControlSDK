#pragma once
#include "../DeviceBase.h"

ECCS_BEGIN

class ISound_Device : public DeviceBase
{
public:
    virtual void OnPacketReceived(std::shared_ptr<rpc::RpcPacket> pkt) override {
        u32 id = pkt->GetID();

        // 播放文件
        if (id == rpc::RqSoundPlay::_FACTORY_ID_) {
            auto req = std::dynamic_pointer_cast<rpc::RqSoundPlay>(pkt);
            PlayFile(req->data.filename, req->data.loop > 0);
            return;
        }

        // 停止
        if (id == rpc::RqSoundStop::_FACTORY_ID_) {
            StopPlay();
            return;
        }

        // TTS
        if (id == rpc::RqSoundTTS::_FACTORY_ID_) {
            auto req = std::dynamic_pointer_cast<rpc::RqSoundTTS>(pkt);
            TTSPlay(req->data.text);
            return;
        }

        // 喊话
        if (id == rpc::RqSoundMic::_FACTORY_ID_) {
            auto req = std::dynamic_pointer_cast<rpc::RqSoundMic>(pkt);
            SetMic(req->data); // bool
            return;
        }

    }

protected:
    // === 纯虚接口 ===
    virtual void PlayFile(const char* filename, bool loop) = 0;
    virtual void StopPlay() = 0;
    virtual void TTSPlay(const char* text) = 0;
    virtual void SetMic(bool isOpen) = 0;
};

ECCS_END