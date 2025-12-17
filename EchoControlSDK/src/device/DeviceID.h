#pragma once
#include "../global.h"

ECCS_BEGIN

// 宏：快速构建 ID
// Type (8bit) | Model (16bit) | Index (8bit)
#define MAKE_DEV_OID(type, model)      u32( (u8(type) << 24) | (u16(model) << 8) )
#define MAKE_DEV_SID(type, model, idx) u32( MAKE_DEV_OID(type, model) | u8(idx) )

namespace did {
    // -------------------------------------------------
    // 设备大类 (DeviceType) - 8 Bit (1~255)
    // -------------------------------------------------
    enum DeviceType
    {
        DEVICE_UNKNOWN = 0,
        DEVICE_LIGHT = 1,
        DEVICE_SOUND = 2,
        DEVICE_PTZ = 3,
        DEVICE_RELAY = 4,
        DEVICE_CAMERA = 5
        /* MaxValue: 0xFF */
    };

    // -------------------------------------------------
    // 具体型号 (Model) - 16 Bit (1~65535)
    // -------------------------------------------------
    enum LightModel
    {
        LIGHT_UNKNOWN = 0,
        LIGHT_PT_LD_1307 = 1,
        LIGHT_HL_525_4W = 2
    };

    enum SoundModel
    {
        SOUND_UNKNOWN = 0,
        SOUND_NETSPEAKER_V2 = 1
    };

    enum PTZModel
    {
        PTZ_UNKNOWN = 0,
        PTZ_YZ_BY010W = 1
    };

    enum RelayModel
    {
        RELAY_UNKNOWN = 0,
        RELAY_TAS_IO_428R2 = 1
    };

    enum CameraModel
    {
        CAMERA_UNKNOWN = 0,
        CAMERA_HKVISION = 1,
        CAMERA_HSXVISION = 2
    };

    const char* DeviceTypeStr(DeviceType type);
    const char* LightModelStr(LightModel model);
    const char* SoundModelStr(SoundModel model);
    const char* PTZModelStr(PTZModel model);
    const char* RelayModelStr(RelayModel model);
    const char* CameraModelStr(CameraModel model);
}


// -----------------------------------------------------
// DeviceID 类定义 (32位)
// Layout: [Type 8bit] [Model 16bit] [Index 8bit]
// -----------------------------------------------------
class DeviceID {
public:
    static const u32 INVALID_ID = 0xFFFFFFFF;

    // 掩码定义
    static const u32 MASK_TYPE = 0xFF000000;
    static const u32 MASK_MODEL = 0x00FFFF00;
    static const u32 MASK_INDEX = 0x000000FF;

public:
    // 构造函数
    DeviceID() : m_id(0) {}
    DeviceID(u32 id) : m_id(id) {}
    // 常用构造：指定类型和型号，Index默认为0
    DeviceID(did::DeviceType type, u16 model, u8 index = 0) {
        m_id = MAKE_DEV_SID(type, model, index);
    }

    // 获取 OID (Object ID, 去掉 Index，用于类型匹配)
    // 返回值如：0x10020000
    u32 GetOID() const {
        return (m_id & (MASK_TYPE | MASK_MODEL));
    }

    // 静态辅助：从完整ID中提取OID
    static u32 GetOID(u32 fullID) {
        return (fullID & (MASK_TYPE | MASK_MODEL));
    }

    // 静态辅助：从完整ID中提取Index
    static u8 GetIndex(u32 fullID) {
        return static_cast<u8>(fullID & MASK_INDEX);
    }

    // 验证 Index 是否有效 (必须 >= 1)
    bool IsIndexValid() const {
        u8 idx = GetIndex();
        return idx >= 1;
    }

    // 赋值与比较
    DeviceID& operator=(const u32& val) { m_id = val; return *this; }
    bool operator==(const DeviceID& other) const { return m_id == other.m_id; }
    bool operator!=(const DeviceID& other) const { return m_id != other.m_id; }
    bool operator<(const DeviceID& other) const { return m_id < other.m_id; } // 用于 std::map key

    // 获取原始值
    u32 Value() const { return m_id; }

    // --- 字段访问器 ---

    // 获取大类 (高8位)
    did::DeviceType GetDeviceType() const {
        return static_cast<did::DeviceType>((m_id & MASK_TYPE) >> 24);
    }

    // 获取型号 (中间16位)
    u16 GetModel() const {
        return static_cast<u16>((m_id & MASK_MODEL) >> 8);
    }

    // 获取索引 (低8位)
    u8 GetIndex() const {
        return static_cast<u8>(m_id & MASK_INDEX);
    }

    // 获取不带Index的ID (用于类型比较)
    u32 GetObjectID() const {
        return (m_id & (MASK_TYPE | MASK_MODEL));
    }

    // --- 修改器 ---
    void SetDeviceType(did::DeviceType type) {
        m_id = (m_id & ~MASK_TYPE) | (u32(type) << 24);
    }
    void SetModel(u16 model) {
        m_id = (m_id & ~MASK_MODEL) | (u32(model) << 8);
    }
    void SetIndex(u8 idx) {
        m_id = (m_id & ~MASK_INDEX) | u32(idx);
    }

    // --- 类型判断助手 ---
    bool IsLight()  const { return GetDeviceType() == did::DEVICE_LIGHT; }
    bool IsSound()  const { return GetDeviceType() == did::DEVICE_SOUND; }
    bool IsPTZ()    const { return GetDeviceType() == did::DEVICE_PTZ; }
    bool IsRelay()  const { return GetDeviceType() == did::DEVICE_RELAY; }
    bool IsCamera() const { return GetDeviceType() == did::DEVICE_CAMERA; }
    bool IsValid()  const { return m_id != 0; }

    // 检查是否同型号 (忽略Index)
    bool IsSameModel(const DeviceID& other) const {
        return GetObjectID() == other.GetObjectID();
    }

    // --- 字符串输出 ---
    str ToString() const;
    str ToHexString() const;

private:
    u32 m_id;
};

ECCS_END