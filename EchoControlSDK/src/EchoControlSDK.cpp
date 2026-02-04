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
static ConfigManager* SafeCast(ECCS_HANDLE hDev) {
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
template <typename TPacket, typename TVal>
ECCS_Error PostPkt(ECCS_HANDLE hDev, did::DeviceType type, const TVal& val)
{
    CHECK_INIT_AND_GET_MGR();
    ConfigManager* mgr = SafeCast(hDev);
    if (!mgr) return ECCS_ERR_INVALID_PARAM;

    DeviceBase* dev = mgr->GetBestDevice(type);
    if (!dev) return ECCS_ERR_DEV_NOT_FOUND; // 找不到对应的硬件模块
    
    auto pkt = std::make_shared<TPacket>(val);
    dev->ExecutePacket(pkt);

    return ECCS_SUCCESS;
}

// --- 接口实现 ---

extern "C" {

    ECCS_API const char* ECCS_GetVersion() {
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

    ECCS_API ECCS_HANDLE ECCS_GetHandle() {
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

    ECCS_API bool ECCS_IsOnline(ECCS_HANDLE hDev) {
        ConfigManager* mgr = SafeCast(hDev);
        if (!mgr) return false;
        // 作为中控代理，如果没有任何一个核心设备在线，则返回 false
        return GET_MGR()->GetDeviceCount() > 0;
    }

    // --- Light ---
    ECCS_API ECCS_Error ECCS_Light_SetSwitch(ECCS_HANDLE hDev, int isOpen)
    {
        return PostPkt<rpc::RqLightSwitch>(hDev, did::DEVICE_LIGHT, (bool)(isOpen != 0));
    }

    ECCS_API ECCS_Error ECCS_Light_SetMode(ECCS_HANDLE hDev, int mode) {
        return PostPkt<rpc::RqLightWorkMode>(hDev, did::DEVICE_LIGHT, (u8)mode);
    }

    ECCS_API ECCS_Error ECCS_Light_SetLevel(ECCS_HANDLE hDev, int level)
    {
        return PostPkt<rpc::RqSetLightLevel>(hDev, did::DEVICE_LIGHT, (u8)level);
    }

    ECCS_API ECCS_Error ECCS_Light_SetFocus(ECCS_HANDLE hDev, int type, int value) {
        rpc::LightFocus data = { (u8)type, (u16)value };
        return PostPkt<rpc::RqLightFocus>(hDev, did::DEVICE_LIGHT, data);
    }

    ECCS_API ECCS_Error ECCS_Light_SetStrobe(ECCS_HANDLE hDev, int isOpen)
    {
        return PostPkt<rpc::RqLightStrobe>(hDev, did::DEVICE_LIGHT, (bool)(isOpen != 0));
    }

    ECCS_API ECCS_Error ECCS_Light_QueryStatus(ECCS_HANDLE hDev) {
        return PostPkt<rpc::RqQueryLightStatus>(hDev, did::DEVICE_LIGHT, rpc::NoneData());
    }

    // --- PTZ ---
    ECCS_API ECCS_Error ECCS_PTZ_Move(ECCS_HANDLE hDev, int action, int speed) 
    {
        rpc::PtzMotion data = { (u8)action, (u8)speed };
        return PostPkt<rpc::RqPtzMove>(hDev, did::DEVICE_PTZ, data);
    }

    ECCS_API ECCS_Error ECCS_PTZ_SetAbsolutePos(ECCS_HANDLE hDev, float pan, float tilt) {
        rpc::PtzPosition data = { pan, tilt, 0 };
        return PostPkt<rpc::RqPtzAbsolutePos>(hDev, did::DEVICE_PTZ, data);
    }

    ECCS_API ECCS_Error ECCS_PTZ_SetScanRange(ECCS_HANDLE hDev, float start, float end) {
        rpc::PtzScanRange data = { start, end };
        return PostPkt<rpc::RqSetPtzScanRange>(hDev, did::DEVICE_PTZ, data);
    }

    ECCS_API ECCS_Error ECCS_PTZ_StartScan(ECCS_HANDLE hDev) {
        return PostPkt<rpc::RqPtzStartScan>(hDev, did::DEVICE_PTZ, rpc::NoneData());
    }

    ECCS_API ECCS_Error ECCS_PTZ_StopScan(ECCS_HANDLE hDev) {
        return PostPkt<rpc::RqPtzStopScan>(hDev, did::DEVICE_PTZ, rpc::NoneData());
    }

    ECCS_API ECCS_Error ECCS_PTZ_Reset(ECCS_HANDLE hDev) {
        return PostPkt<rpc::RqPtzReset>(hDev, did::DEVICE_PTZ, rpc::NoneData());
    }

    // --- Sound ---
    ECCS_API ECCS_Error ECCS_Sound_Play(ECCS_HANDLE hDev, int index, int loop) 
    {
        rpc::SoundPlayIndex data = { index, (u8)loop };
        return PostPkt<rpc::RqSoundPlayIndex>(hDev, did::DEVICE_SOUND, data);
    }

    ECCS_API ECCS_Error ECCS_Sound_Stop(ECCS_HANDLE hDev) 
    {
        return PostPkt<rpc::RqSoundStop>(hDev, did::DEVICE_SOUND, rpc::NoneData());
    }

    ECCS_API ECCS_Error ECCS_Sound_Next(ECCS_HANDLE hDev) {
        return PostPkt<rpc::RqSoundNext>(hDev, did::DEVICE_SOUND, rpc::NoneData());
    }

    ECCS_API ECCS_Error ECCS_Sound_Prev(ECCS_HANDLE hDev) {
        return PostPkt<rpc::RqSoundPrev>(hDev, did::DEVICE_SOUND, rpc::NoneData());
    }

    ECCS_API ECCS_Error ECCS_Sound_OneKeyPlay(ECCS_HANDLE hDev, int index) {
        return PostPkt<rpc::RqSoundOneKey>(hDev, did::DEVICE_SOUND, index);
    }

    ECCS_API ECCS_Error ECCS_Sound_SetCapVolume(ECCS_HANDLE hDev, int volume) 
    {
        return PostPkt<rpc::RqSetSoundCapVolume>(hDev, did::DEVICE_SOUND, (u8)volume);
    }

    ECCS_API ECCS_Error ECCS_Sound_SetPlayVolume(ECCS_HANDLE hDev, int volume)
    {
        return PostPkt<rpc::RqSetSoundPlayVolume>(hDev, did::DEVICE_SOUND, (u8)volume);
    }

    ECCS_API ECCS_Error ECCS_Sound_QueryAudioList(ECCS_HANDLE hDev) {
        return PostPkt<rpc::RqQueryAudioList>(hDev, did::DEVICE_SOUND, rpc::NoneData());
    }

    ECCS_API ECCS_Error ECCS_Sound_SetMic(ECCS_HANDLE hDev, int isOpen) 
    {
        return PostPkt<rpc::RqSoundMicSwitch>(hDev, did::DEVICE_SOUND, (bool)(isOpen != 0));
    }

    ECCS_API ECCS_Error ECCS_Sound_PushData(ECCS_HANDLE hDev, const char* data, int len) {
        CHECK_INIT_AND_GET_MGR();
        // 实时音频流由于高频，绕过 RPC 队列直接推送到驱动 Buffer
        DeviceBase* dev = mgr->GetBestDevice(did::DEVICE_SOUND);
        auto soundDev = dynamic_cast<ISound_Device*>(dev);
        if (soundDev) 
            return soundDev->PushAudio((const u8*)data, (u32)len);
        return ECCS_ERR_DEV_NOT_FOUND;
    }

    ECCS_API ECCS_Error ECCS_Ultrasonic_SetSwitch(ECCS_HANDLE hSystem, int channel, int isOpen)
    {
        rpc::UltrasonicSwitch data;
        data.channel = (u8)channel;
        data.isOpen = (u8)(isOpen != 0);
        return PostPkt<rpc::RqUltrasonicSwitch>(hSystem, did::DEVICE_ULTRASONIC, data);
    }

}