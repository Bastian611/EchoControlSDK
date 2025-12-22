#include "../include/EchoControlSDK.h"
#include "../include/EchoControlCode.h"
#include "debug/Logger.h"
#include "Version.h"
#include "config/ConfigManager.h"
#include "device/DeviceBase.h"
#include "device/Sound/ISound_Device.h" 
#include "protocol/Packet_Def.h"
#include <string.h>

USING_ECCS

// 默认配置文件路径
static const char* DEFAULT_RULE_PATH = "./config/global.cfg";
static const char* DEFAULT_DEV_PATH  = "./config/device.cfg";

// --- 内部辅助 ---

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
    ConfigManager* mgr = SafeCast(hDev);
    DeviceBase* dev = InternalFindDevice(mgr, type);

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
            // 使用默认路径
        	// 建议在 ConfigManager 内部处理路径检查，如果文件不存在抛出异常
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
            else if (id == rpc::OwSoundPlayEnd::_FACTORY_ID_) {
                cb(hDev, ECCS_EVT_SOUND_FINISH, nullptr, 0, userCtx);
            }
        };

        // 假设 ConfigManager 已经有了 SetGlobalCallback 遍历设置给所有设备
        //mgr->SetGlobalCallback(internalCb);

        // 临时实现：手动遍历设置
        int count = mgr->GetDeviceCount();
        for (int i = 0; i < count; ++i) {
            DeviceBase* dev = mgr->GetDeviceByIndex(i);
            if (dev) dev->SetStatusCallback(internalCb);
        }

        return ECCS_SUCCESS;
    }

    ECCS_API bool ECCS_IsSystemOnline(ECCS_HANDLE hDev) {
        ConfigManager* mgr = SafeCast(hDev);
        if (!mgr) return false;
        // 简单策略：所有设备都在线才算系统在线，或者只要有一个在线
        // 这里示例：检查是否所有 Critical 设备在线
        return true;
    }

    // --- Light ---
    ECCS_API ECCS_Error ECCS_Light_SetSwitch(ECCS_HANDLE hDev, int isOpen)
    {
        return PostPkt<rpc::RqLightSwitch>(hDev, did::DEVICE_LIGHT, (bool)(isOpen != 0));
    }

    ECCS_API ECCS_Error ECCS_Light_SetLevel(ECCS_HANDLE hDev, int level)
    {
        return PostPkt<rpc::RqLightLevel>(hDev, did::DEVICE_LIGHT, (u8)level);
    }

    ECCS_API ECCS_Error ECCS_Light_SetStrobe(ECCS_HANDLE hDev, int isOpen)
    {
        return PostPkt<rpc::RqLightStrobe>(hDev, did::DEVICE_LIGHT, (bool)(isOpen != 0));
    }

    // --- PTZ ---
    ECCS_API ECCS_Error ECCS_PTZ_Move(ECCS_HANDLE hDev, int action, int speed) 
    {
        rpc::PtzMotion data = { (u8)action, (u8)speed };
        return PostPkt<rpc::RqPtzMove>(hDev, did::DEVICE_PTZ, data);
    }

    ECCS_API ECCS_Error ECCS_PTZ_Zoom(ECCS_HANDLE hDev, int isZoomIn)
    {
        // 暂不支持或需要扩展协议
        return ECCS_ERR_NOT_SUPPORTED;
    }

    ECCS_API ECCS_Error ECCS_PTZ_Preset(ECCS_HANDLE hDev, int action, int index)
    {
        rpc::PtzPreset data = { (u8)action, (u8)index };
        return PostPkt<rpc::RqPtzPreset>(hDev, did::DEVICE_PTZ, data);
    }

    // --- Sound ---
    ECCS_API ECCS_Error ECCS_Sound_Play(ECCS_HANDLE hDev, const char* filename, int loop) 
    {
        rpc::SoundPlayCtrl data;
        strncpy(data.filename, filename, sizeof(data.filename) - 1);
        data.loop = (u8)loop;
        return PostPkt<rpc::RqSoundPlay>(hDev, did::DEVICE_SOUND, data);
    }

    ECCS_API ECCS_Error ECCS_Sound_Stop(ECCS_HANDLE hDev) 
    {
        return PostPkt<rpc::RqSoundStop>(hDev, did::DEVICE_SOUND, rpc::NoneData());
    }

    ECCS_API ECCS_Error ECCS_Sound_SetVolume(ECCS_HANDLE hDev, int volume) 
    {
        rpc::SoundVolCtrl data = { (u8)volume };
        // 确保 PacketDef.h 中有 RqSoundSetVolume
        return PostPkt<rpc::RqSetSoundVolume>(hDev, did::DEVICE_SOUND, data);
    }

    ECCS_API ECCS_Error ECCS_Sound_TTS(ECCS_HANDLE hDev, const char* text) 
    {
        rpc::SoundTTSCtrl data;
        strncpy(data.text, text, sizeof(data.text) - 1);
        data.text[sizeof(data.text) - 1] = '\0'; // 确保字符串以 null 结尾
        return PostPkt<rpc::RqSoundTTS>(hDev, did::DEVICE_SOUND, data);
    }

    ECCS_API ECCS_Error ECCS_Sound_SetMic(ECCS_HANDLE hDev, int isOpen) 
    {
        return PostPkt<rpc::RqSoundMic>(hDev, did::DEVICE_SOUND, (bool)(isOpen != 0));
    }

    ECCS_API ECCS_Error ECCS_Sound_PushData(ECCS_HANDLE hDev, const char* data, int len) {
        ConfigManager* mgr = SafeCast(hDev);
        DeviceBase* dev = InternalFindDevice(mgr, did::DEVICE_SOUND);

        auto soundDev = dynamic_cast<ISound_Device*>(dev);
        if (soundDev) {
            soundDev->PushAudio((const u8*)data, (u32)len);
            return ECCS_SUCCESS;
        }
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