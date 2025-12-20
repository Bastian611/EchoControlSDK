#include "../include/EchoControlSDK.h"
#include "Version.h"
#include "config/ConfigManager.h"
#include "device/DeviceBase.h"
#include "device/Sound/ISound_Device.h" // 为了 Cast 获取端口
#include "protocol/Packet_Def.h"
#include <string.h>

USING_ECCS

// 默认配置文件路径
static const char* DEFAULT_RULE_PATH = "./config/system_rules.sys";
static const char* DEFAULT_DEV_PATH  = "./config/device_params.dev";

// --- 内部辅助 ---

// 安全转换句柄
static DeviceBase* SafeCast(ECCS_HANDLE hDev) {
    if (hDev == ECCS_INVALID_HANDLE) return nullptr;
    return static_cast<DeviceBase*>(hDev);
}

// 检查设备类型
static bool CheckType(DeviceBase* dev, did::DeviceType type) {

    return dev && dev->GetDeviceID().GetDeviceType() == type;
}

// 构造并发送包
template <typename TPacket, typename TVal>
void PostPkt(DeviceBase* dev, const TVal& val) {
    // 1. 创建智能指针
    // std::make_shared 会尝试寻找 TPacket(const TVal&) 的构造函数
    // 这对于 struct、bool、u8 等类型都适用
    auto pkt = std::make_shared<TPacket>(val);

    // 2. 发送
    // 这里利用了多态：shared_ptr<TPacket> -> shared_ptr<RpcPacket>
    dev->ExecutePacket(pkt);
}

// --- 接口实现 ---

extern "C" {

    ECCS_API const char* ECCS_GetVersion() {
        return ECCS_VER_STR; // 使用 Version.h 中的宏
    }

    ECCS_API ECCS_ReturnCode ECCS_Init()
    {
        try {
            // 使用默认路径
        	// 建议在 ConfigManager 内部处理路径检查，如果文件不存在抛出异常
        	ConfigManager::getInstance()->LoadSystem(DEFAULT_RULE_PATH, DEFAULT_DEV_PATH);
        	return ECCS_RET_SUCCESS;
        }
        catch (...) {
            ECCS_FAILURE;
        }
    }

    ECCS_API void ECCS_Release() 
    {
        ConfigManager::getInstance()->Release();
    }

    // 获取设备数量
    ECCS_API int ECCS_GetDeviceCount() 
    {
        return ConfigManager::getInstance()->GetDeviceCount();
    }

    // 通过索引获取句柄
    ECCS_API ECCS_HANDLE ECCS_GetDeviceByIndex(int index) 
    {
        // 需要在 ConfigManager 中实现 GetDeviceByIndex
        DeviceBase* dev = ConfigManager::getInstance()->GetDeviceByIndex(index);
        return (ECCS_HANDLE)dev;
    }

    // 获取设备类型
    ECCS_API int ECCS_GetDeviceType(ECCS_HANDLE hDev) 
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!dev) return ECCS_DEV_UNKNOWN;
        return (int)dev->GetDeviceID().GetDeviceType();
    }

    ECCS_API ECCS_ReturnCode ECCS_RegisterCallback(ECCS_HANDLE hDev, ECCS_CallbackFunc cb, void* userCtx) 
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!dev) return ECCS_NOT_INITIALIZED;

        // 设置 C++ 回调，并在内部转调 C 函数指针
        dev->SetStatusCallback([cb, userCtx, hDev](std::shared_ptr<rpc::RpcPacket> pkt) {
            if (!cb || !pkt) return;

            u32 id = pkt->GetID();

            // 1. 状态变更 (OwDeviceStatus)
            if (id == rpc::OwDeviceStatus::_FACTORY_ID_) {
                auto p = std::dynamic_pointer_cast<rpc::OwDeviceStatus>(pkt);
                if (p) {
                    cb(hDev, ECCS_EVT_STATUS_CHANGE, &p->data, sizeof(p->data), userCtx);
                }
            }
            // 2. 云台角度 (OwPtzPosition)
            else if (id == rpc::OwPtzPosition::_FACTORY_ID_) {
                auto p = std::dynamic_pointer_cast<rpc::OwPtzPosition>(pkt);
                if (p) {
                    cb(hDev, ECCS_EVT_PTZ_REPORT, &p->data, sizeof(p->data), userCtx);
                }
            }
            // 3. 强声播放结束
            else if (id == rpc::OwSoundPlayEnd::_FACTORY_ID_) {
                cb(hDev, ECCS_EVT_SOUND_END, nullptr, 0, userCtx);
            }
            });

        return ECCS_ReturnCode::ECCS_RET_SUCCESS;
    }

// [修改] 设置配置：通过句柄反查内部 SlotID，用户无感知
    ECCS_API ECCS_ReturnCode ECCS_SetConfig(ECCS_HANDLE hDev, const char* key, const char* value) 
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!dev) return ECCS_NOT_INITIALIZED;

        // 1. 获取设备内部记录的 slotID
        // 需要确保 DeviceBase.h 中有 public: int GetSlotID() const { return m_slotID; }
        int internalSlot = dev->GetSlotID(); 
    
        // 2. 调用 ConfigManager 落盘
        if (ConfigManager::getInstance()->UpdateConfig(internalSlot, key, value)) {
            return ECCS_RET_SUCCESS;
        }
        return ECCS_FAILURE; 
    }

    ECCS_API ECCS_ReturnCode ECCS_GetConfig(ECCS_HANDLE hDev, const char* key, char* outBuf, int maxLen) 
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!dev) return ECCS_NOT_INITIALIZED;
    
        str val = dev->GetProperty(key);
        strncpy(outBuf, val.c_str(), maxLen - 1);
        outBuf[maxLen - 1] = '\0';
        return ECCS_RET_SUCCESS;
    }

    // --- Light ---
    ECCS_API ECCS_ReturnCode ECCS_Light_SetSwitch(ECCS_HANDLE hDev, int isOpen)
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_LIGHT)) return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;

        rpc::LightStatus data; // 注意：复用 LightStatus 或定义专用 Switch 结构
        // 假设 PacketDef 定义的是 bool 类型：
        // auto pkt = std::make_shared<rpc::RqLightSwitch>(isOpen != 0);
        // 你的定义是: typedef Packet<..., bool> RqLightSwitch;

        PostPkt<rpc::RqLightSwitch>(dev, (isOpen != 0));
        return ECCS_ReturnCode::ECCS_RET_SUCCESS;
    }

    ECCS_API ECCS_ReturnCode ECCS_Light_SetLevel(ECCS_HANDLE hDev, int level)
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_LIGHT)) return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;

        PostPkt<rpc::RqLightLevel>(dev, (u8)level);
        return ECCS_ReturnCode::ECCS_RET_SUCCESS;
    }

    ECCS_API ECCS_ReturnCode ECCS_Light_SetStrobe(ECCS_HANDLE hDev, int isOpen)
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_LIGHT)) return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;

        PostPkt<rpc::RqLightStrobe>(dev, (isOpen != 0));
        return ECCS_ReturnCode::ECCS_RET_SUCCESS;
    }

    // --- PTZ ---
    ECCS_API ECCS_ReturnCode ECCS_PTZ_Move(ECCS_HANDLE hDev, int action, int speed) 
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_PTZ)) return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;

        rpc::PtzMotion data;
        data.action = (u8)action;
        data.speed = (u8)speed;
        PostPkt<rpc::RqPtzMove>(dev, data);
        return ECCS_ReturnCode::ECCS_RET_SUCCESS;
    }

    ECCS_API ECCS_ReturnCode ECCS_PTZ_Zoom(ECCS_HANDLE hDev, int isZoomIn)
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_PTZ)) return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;

        // 复用 Move 指令的 Action 定义，假设 6=ZoomIn, 7=ZoomOut
        // 或者 PtzMotion 结构体不够，需要在 PacketDef.h 增加 RqPtzZoom
        // 暂时用 PtzMove 占位
        return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;
    }

    ECCS_API ECCS_ReturnCode ECCS_PTZ_Preset(ECCS_HANDLE hDev, int action, int index)
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_PTZ)) return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;

        rpc::PtzPreset data;
        data.action = (u8)action;
        data.index = (u8)index;
        PostPkt<rpc::RqPtzPreset>(dev, data);
        return ECCS_ReturnCode::ECCS_RET_SUCCESS;
    }

    // --- Sound ---
    ECCS_API ECCS_ReturnCode ECCS_Sound_Play(ECCS_HANDLE hDev, const char* filename, int loop) 
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_SOUND)) return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;

        rpc::SoundPlayCtrl data;
        strncpy(data.filename, filename, sizeof(data.filename) - 1);
        data.loop = (u8)loop;
        PostPkt<rpc::RqSoundPlay>(dev, data);
        return ECCS_ReturnCode::ECCS_RET_SUCCESS;
    }

    ECCS_API ECCS_ReturnCode ECCS_Sound_Stop(ECCS_HANDLE hDev) 
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_SOUND)) return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;

        rpc::NoneData data;
        PostPkt<rpc::RqSoundStop>(dev, data);
        return ECCS_ReturnCode::ECCS_RET_SUCCESS;
    }

    ECCS_API ECCS_ReturnCode ECCS_Sound_SetVolume(ECCS_HANDLE hDev, int volume) 
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_SOUND)) return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;

        rpc::SoundVolCtrl data;
        data.volume = (u8)volume;
        // 假设 PacketDef 中有 RqSoundVol
        // PostPkt<rpc::RqSoundVol>(dev, data); 
        return ECCS_ReturnCode::ECCS_RET_SUCCESS;
    }

    ECCS_API ECCS_ReturnCode ECCS_Sound_TTS(ECCS_HANDLE hDev, const char* text) 
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_SOUND)) return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;

        rpc::SoundTTSCtrl data;
        strncpy(data.text, text, sizeof(data.text) - 1);
        PostPkt<rpc::RqSoundTTS>(dev, data);
        return ECCS_ReturnCode::ECCS_RET_SUCCESS;
    }

    ECCS_API ECCS_ReturnCode ECCS_Sound_SetMic(ECCS_HANDLE hDev, int isOpen) 
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_SOUND)) return ECCS_ReturnCode::ECCS_UNSUPPORTED_OPERATION;

        bool data = (isOpen != 0);
        PostPkt<rpc::RqSoundMic>(dev, data);
        return ECCS_ReturnCode::ECCS_RET_SUCCESS;
    }

    ECCS_API int ECCS_Sound_GetStreamPort(ECCS_HANDLE hDev) 
    {
        DeviceBase* dev = SafeCast(hDev);
        if (!CheckType(dev, did::DEVICE_SOUND)) return 0;

        // 强转为 ISound_Device 接口获取端口
        // 这里需要引入 ISound_Device.h
        auto soundDev = dynamic_cast<ISound_Device*>(dev);
        if (soundDev) {
            // 假设我们在 ISound_Device 中添加了 GetProxyPort() 接口
            // return soundDev->GetProxyPort();
            return 10000 + dev->GetDeviceID().GetIndex(); // 临时实现
        }
        return 0;
    }

}