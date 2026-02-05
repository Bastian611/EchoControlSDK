#include "../include/EchoControlSDK.h"
#include "../include/EchoControlCode.h"
#include "debug/Logger.h"
#include "Version.h"
#include "config/ConfigManager.h"
#include "device/DeviceBase.h"
#include "device/Sound/ISound_Device.h" 
#include "device/Light/ILight_Device.h"
#include "device/PTZ/IPTZ_Device.h"
#include "protocol/Packet_Def.h"
#include "utils/file_system.h"
#include <string.h>

USING_ECCS

#define GET_MGR() ConfigManager::getInstance()

// 核心安全检查：确保 SDK 已初始化
#define CHECK_INIT_AND_GET_MGR() \
    if (!GET_MGR()->IsInitialized()) return ECCS_ERR_NOT_INIT; \
    ConfigManager* mgr = GET_MGR();

// 默认配置文件路径
static const char* DEFAULT_RULE_PATH = "./config/global.cfg";
static const char* DEFAULT_DEV_PATH  = "./config/device.cfg";

// 字符串安全拷贝
#define SAFE_STRCPY(dst, src) \
    do { \
        strncpy(dst, src, sizeof(dst) - 1); \
        (dst)[sizeof(dst) - 1] = '\0'; \
    } while(0)

// 安全转换句柄
static ConfigManager* SafeCast(ECCS_HANDLE hDev) 
{
    if (hDev == ECCS_INVALID_HANDLE) return nullptr;
    return static_cast<ConfigManager*>(hDev);
}

static DeviceBase* InternalFindDevice(ConfigManager* mgr, did::DeviceType type) 
{
    if (!mgr) return nullptr;

    // 遍历 ConfigManager 管理的所有设备
    // 注意：ConfigManager 需要提供遍历接口，或者我们利用 GetDeviceCount/GetDeviceByIndex
    int count = mgr->GetDeviceCount();
    for (int i = 0; i < count; ++i) {
        DeviceBase* dev = mgr->GetDeviceByIndex(i);
        if (!dev) continue;

        // 获取设备的完整 ID
        DeviceID id = dev->GetDeviceID();

        // 先匹配大类 (Light/Sound/PTZ)
        if (id.GetDeviceType() != type) continue;

        // [进阶判断] 如果系统里有多个同类设备，在这里通过 Model 或 Slot 区分
        // 例如：只控制 Slot 1 的灯，或者只控制 HL-525 型号的灯
        // if (type == did::DEVICE_LIGHT && dev->GetSlotID() != 1) continue; 

        // 目前策略：返回找到的第一个该类型的设备 (通常系统里每种主设备只有一个)
        return dev;
    }
    return nullptr;
}

// 构造并发送包
template <typename TRqPacket, typename TRpPacket, typename TVal, typename TResult>
ECCS_Error PostPkt(ECCS_HANDLE hDev, did::DeviceType type, const TVal& val, TResult* outData)
{
    CHECK_INIT_AND_GET_MGR();
    ConfigManager* mgr = SafeCast(hDev);
    if (!mgr) 
        return ECCS_ERR_INVALID_PARAM;

    DeviceBase* dev = mgr->GetBestDevice(type);
    if (!dev) 
        return ECCS_ERR_DEV_NOT_FOUND; // 找不到对应的硬件模块

    // 创建局部信号量用于同步等待
    Semaphore syncSem(0);
    TResult result;
    std::atomic<ECCS_Error> ret{ ECCS_ERR_TIMEOUT };
    
    auto oldCb = dev->GetStatusCallback();

    // 注册临时回调监听 Rp 包
    auto tempCb = [&](std::shared_ptr<rpc::RpcPacket> pkt) {
        if (pkt->GetID() == TRpPacket::_FACTORY_ID_) {
            auto p = std::dynamic_pointer_cast<TRpPacket>(pkt);
            if (p) {
                result = p->data;
                ret = ECCS_SUCCESS;
                syncSem.notify();
            }
        }

        // 如果有旧回调，也顺便触发它，保证全局监听不中断
        if (oldCb) oldCb(pkt);
    };

    // 挂载回调并执行发送
    dev->SetStatusCallback(tempCb);
    
    auto pkt = std::make_shared<TRqPacket>(val);
    dev->ExecutePacket(pkt);

    // 阻塞等待 500ms
    if (syncSem.wait_for(ECCS_C11 chrono::milliseconds(500))) {
        if (outData)
            *outData = result;
        // 恢复全局状态监听 (重要：防止干扰后续流程)
        // mgr->SetGlobalCallback(...) 会在外部重新设置回来的
        return ret.load();
    }

    // 还原回调
    dev->SetStatusCallback(oldCb);

    return ret;
}

// --- 接口实现 ---

extern "C" {

    ECCS_API const char* ECCS_GetVersion() 
    {
        return ECCS_VER_STR; // 使用 Version.h 中的宏
    }

    ECCS_API ECCS_Error ECCS_Init()
    {
        try {
        	ConfigManager::getInstance()->LoadSystem(DEFAULT_RULE_PATH, DEFAULT_DEV_PATH);
        	return ECCS_SUCCESS;
        }
        catch (...) {
            LOG_ERROR("Init Failed: %s", ECCS_GetErrorStr(ECCS_ERR_CFG_LOAD_FAILED))
            return ECCS_ERR_CFG_LOAD_FAILED;
        }
    }

    ECCS_API void ECCS_Release() 
    {
        ConfigManager::getInstance()->Release();
    }

    ECCS_API ECCS_HANDLE ECCS_GetHandle() 
    {
        // 返回 ConfigManager 单例作为系统句柄
        return (ECCS_HANDLE)ConfigManager::getInstance();
    }

    ECCS_API ECCS_Error ECCS_RegisterCallback(ECCS_HANDLE hDev, ECCS_CallbackFunc cb, void* userCtx) 
    {
        ConfigManager* mgr = SafeCast(hDev);
        if (!mgr) return ECCS_ERR_NOT_INIT;

        // 定义 lambda 转换层
        auto internalCb = [cb, userCtx, hDev](std::shared_ptr<rpc::RpcPacket> pkt) {
            if (!cb || !pkt) return;
            u32 id = pkt->GetID();

            if (id == rpc::OwDeviceStatus::_FACTORY_ID_) {
                auto p = std::dynamic_pointer_cast<rpc::OwDeviceStatus>(pkt);
                if (p) cb(hDev, ECCS_EVT_STATUS_CHANGE, &p->data, sizeof(p->data), userCtx);
            }
            else if (id == rpc::OwPtzPosition::_FACTORY_ID_) {
                auto p = std::dynamic_pointer_cast<rpc::OwPtzPosition>(pkt);
                if (p) cb(hDev, ECCS_EVT_PTZ_ANGLE, &p->data, sizeof(p->data), userCtx);
            }
            else if (id == rpc::OwSoundPlayStatus::_FACTORY_ID_) {
                auto p = std::dynamic_pointer_cast<rpc::OwSoundPlayStatus>(pkt);
                // 仅当播放完成时通知应用层
                if (p && p->data.playState == SoundPlayState::Finished)
                cb(hDev, ECCS_EVT_SOUND_FINISH, nullptr, 0, userCtx);
            }
        };

        mgr->SetGlobalCallback(internalCb);
        return ECCS_SUCCESS;
    }

    ECCS_API bool ECCS_IsOnline(ECCS_HANDLE hDev) 
    {
        ConfigManager* mgr = SafeCast(hDev);
        if (!mgr) return false;
        // 作为中控代理，如果没有任何一个核心设备在线，则返回 false
        return GET_MGR()->GetDeviceCount() > 0;
    }

    ECCS_API ECCS_Error ECCS_Device_Disconnect(ECCS_HANDLE hDev) 
    {
        CHECK_INIT_AND_GET_MGR();
        // 遍历所有设备并停止
        for (int i = 0; i < mgr->GetDeviceCount(); ++i) {
            auto dev = mgr->GetDeviceByIndex(i);
            if (dev) dev->Stop();
        }
        return ECCS_SUCCESS;
    }

    // --- Light ---
    ECCS_API ECCS_Error ECCS_Light_SetSwitch(ECCS_HANDLE hDev, int isOpen)
    {
        rpc::Result result;
        return PostPkt<rpc::RqLightSwitch, rpc::RpLightSwitch>(hDev, did::DEVICE_LIGHT, (bool)(isOpen != 0), &result);
    }

    ECCS_API ECCS_Error ECCS_Light_SetMode(ECCS_HANDLE hDev, int mode) 
    {
        rpc::Result result;
        return PostPkt<rpc::RqLightWorkMode, rpc::RpLightWorkMode>(hDev, did::DEVICE_LIGHT, (u8)mode, &result);
    }

    ECCS_API ECCS_Error ECCS_Light_SetLevel(ECCS_HANDLE hDev, int level)
    {
        rpc::Result result;
        return PostPkt<rpc::RqSetLightLevel, rpc::RpSetLightLevel>(hDev, did::DEVICE_LIGHT, (u8)level, &result);
    }

    ECCS_API ECCS_Error ECCS_Light_SetFocus(ECCS_HANDLE hDev, int type, int value) 
    {
        rpc::Result result;
        rpc::LightFocus data = { (u8)type, (u16)value };
        return PostPkt<rpc::RqLightFocus, rpc::RpLightFocus>(hDev, did::DEVICE_LIGHT, data, &result);
    }

    ECCS_API ECCS_Error ECCS_Light_SetStrobe(ECCS_HANDLE hDev, int isOpen)
    {
        rpc::Result result;
        return PostPkt<rpc::RqLightStrobe, rpc::RpLightStrobe>(hDev, did::DEVICE_LIGHT, (bool)(isOpen != 0), &result);
    }

    // --- PTZ ---
    ECCS_API ECCS_Error ECCS_PTZ_Move(ECCS_HANDLE hDev, int action, int speed) 
    {
        rpc::Result result;
        rpc::PtzMotion data = { (u8)action, (u8)speed };
        return PostPkt<rpc::RqPtzMove, rpc::RpPtzMove>(hDev, did::DEVICE_PTZ, data, &result);
    }

    ECCS_API ECCS_Error ECCS_PTZ_SetAbsolutePos(ECCS_HANDLE hDev, float pan, float tilt) 
    {
        rpc::Result result;
        rpc::PtzPosition data = { pan, tilt, 0 };
        return PostPkt<rpc::RqPtzAbsolutePos, rpc::RpPtzAbsolutePos>(hDev, did::DEVICE_PTZ, data, &result);
    }

    ECCS_API ECCS_Error ECCS_PTZ_SetScanRange(ECCS_HANDLE hDev, float start, float end) 
    {
        rpc::Result result;
        rpc::PtzScanRange data = { start, end };
        return PostPkt<rpc::RqSetPtzScanRange, rpc::RpSetPtzScanRange>(hDev, did::DEVICE_PTZ, data, &result);
    }

    ECCS_API ECCS_Error ECCS_PTZ_StartScan(ECCS_HANDLE hDev) 
    {
        rpc::Result result;
        return PostPkt<rpc::RqPtzStartScan, rpc::RpPtzStartScan>(hDev, did::DEVICE_PTZ, rpc::NoneData(), &result);
    }

    ECCS_API ECCS_Error ECCS_PTZ_StopScan(ECCS_HANDLE hDev) 
    {
        rpc::Result result;
        return PostPkt<rpc::RqPtzStopScan, rpc::RpPtzStopScan>(hDev, did::DEVICE_PTZ, rpc::NoneData(), &result);
    }

    ECCS_API ECCS_Error ECCS_PTZ_Reset(ECCS_HANDLE hDev) 
    {
        rpc::Result result;
        return PostPkt<rpc::RqPtzReset, rpc::RpPtzReset>(hDev, did::DEVICE_PTZ, rpc::NoneData(), &result);
    }

    // --- Sound ---
    ECCS_API ECCS_Error ECCS_Sound_Play(ECCS_HANDLE hDev, int index, int loop) 
    {
        rpc::Result result;
        rpc::SoundPlayIndex data = { index, (u8)loop };
        return PostPkt<rpc::RqSoundPlayIndex, rpc::RpSoundPlayIndex>(hDev, did::DEVICE_SOUND, data, &result);
    }

    ECCS_API ECCS_Error ECCS_Sound_Stop(ECCS_HANDLE hDev) 
    {
        rpc::Result result;
        return PostPkt<rpc::RqSoundStop, rpc::RpSoundStop>(hDev, did::DEVICE_SOUND, rpc::NoneData(), &result);
    }

    ECCS_API ECCS_Error ECCS_Sound_Next(ECCS_HANDLE hDev) 
    {
        rpc::Result result;
        return PostPkt<rpc::RqSoundNext, rpc::RpSoundNext>(hDev, did::DEVICE_SOUND, rpc::NoneData(), &result);
    }

    ECCS_API ECCS_Error ECCS_Sound_Prev(ECCS_HANDLE hDev) 
    {
        rpc::Result result;
        return PostPkt<rpc::RqSoundPrev, rpc::RpSoundPrev>(hDev, did::DEVICE_SOUND, rpc::NoneData(), &result);
    }

    ECCS_API ECCS_Error ECCS_Sound_OneKeyPlay(ECCS_HANDLE hDev, int index) 
    {
        rpc::Result result;
        return PostPkt<rpc::RqSoundOneKey, rpc::RpSoundOneKey>(hDev, did::DEVICE_SOUND, index, &result);
    }

    ECCS_API ECCS_Error ECCS_Sound_SetCapVolume(ECCS_HANDLE hDev, int volume) 
    {
        rpc::Result result;
        return PostPkt<rpc::RqSetSoundCapVolume, rpc::RpSetSoundCapVolume>(hDev, did::DEVICE_SOUND, (u8)volume, &result);
    }

    ECCS_API ECCS_Error ECCS_Sound_SetPlayVolume(ECCS_HANDLE hDev, int volume)
    {
        rpc::Result result;
        return PostPkt<rpc::RqSetSoundPlayVolume, rpc::RpSetSoundPlayVolume>(hDev, did::DEVICE_SOUND, (u8)volume, &result);
    }

    ECCS_API ECCS_Error ECCS_Sound_QueryPlayVolume(ECCS_HANDLE hDev, int* volume)
    {
        rpc::Result result;
        return PostPkt<rpc::RqQueryPlayVolume, rpc::RpQueryPlayVolume>(hDev, did::DEVICE_SOUND, rpc::NoneData(), volume);
    }

    ECCS_API ECCS_Error ECCS_Sound_QueryAudioList(ECCS_HANDLE hDev) 
    {
        rpc::Result result;
        return PostPkt<rpc::RqQueryAudioList, rpc::RpQueryAudioList>(hDev, did::DEVICE_SOUND, rpc::NoneData(), &result);
    }

    ECCS_API ECCS_Error ECCS_Sound_SetMic(ECCS_HANDLE hDev, int isOpen) 
    {
        rpc::Result result;
        return PostPkt<rpc::RqSoundMicSwitch, rpc::RpSoundMicSwitch>(hDev, did::DEVICE_SOUND, (bool)(isOpen != 0), &result);
    }

    ECCS_API ECCS_Error ECCS_Sound_PushData(ECCS_HANDLE hDev, const char* data, int len) 
    {
        CHECK_INIT_AND_GET_MGR();
        // 实时音频流由于高频，绕过 RPC 队列直接推送到驱动 Buffer
        DeviceBase* dev = mgr->GetBestDevice(did::DEVICE_SOUND);
        auto soundDev = dynamic_cast<ISound_Device*>(dev);
        if (soundDev) 
            return soundDev->PushAudio((const u8*)data, (u32)len);
        return ECCS_ERR_DEV_NOT_FOUND;
    }

    ECCS_API ECCS_Error ECCS_Sound_UploadFile(ECCS_HANDLE hDev, const char* localPath) 
    {
        CHECK_INIT_AND_GET_MGR();
        DeviceBase* dev = mgr->GetBestDevice(did::DEVICE_SOUND);
        auto soundDev = dynamic_cast<ISound_Device*>(dev);
        if (!soundDev)
            return ECCS_ERR_DEV_NOT_FOUND;

        // 读取文件内容
        std::vector<u8> fileData;
        if (fileSize(localPath) <= 0) 
            return ECCS_ERR_CFG_LOAD_FAILED;

        std::ifstream ifs(localPath, std::ios::binary);
        fileData.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        // 调用驱动层上传接口
        return soundDev->UploadAudioFile(getDir(localPath), fileData.data(), (u32)fileData.size());
    }

    ECCS_API ECCS_Error ECCS_Sound_RegisterAudioCallback(ECCS_HANDLE hDev, ECCSAudioRxCallback cb, void* userCtx) 
    {
        CHECK_INIT_AND_GET_MGR();
        DeviceBase* dev = mgr->GetBestDevice(did::DEVICE_SOUND);
        auto soundDev = dynamic_cast<ISound_Device*>(dev);
        if (soundDev) {
            // 将驱动层的原始 UDP 回调包装后传给用户
            soundDev->SetCaptureCallback([cb, hDev, userCtx](const u8* data, u32 len) {
                if (cb) cb(hDev, data, (int)len, userCtx);
                });
            return ECCS_SUCCESS;
        }
        return ECCS_ERR_DEV_NOT_FOUND;
    }

    // =======================================================
    // 超声接口 (Ultrasonic)
    // =======================================================

    ECCS_API ECCS_Error ECCS_Ultrasonic_SetSwitch(ECCS_HANDLE hSystem, int channel, int isOpen)
    {
        rpc::Result result;
        rpc::UltrasonicSwitch data;
        data.channel = (u8)channel;
        data.isOpen = (u8)(isOpen != 0);
        return PostPkt<rpc::RqUltrasonicSwitch, rpc::RpUltrasonicSwitch>(hSystem, did::DEVICE_ULTRASONIC, data, &result);
    }

}