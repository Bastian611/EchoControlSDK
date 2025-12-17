#include "DeviceID.h"
#include <cstdio>

ECCS_BEGIN

namespace did {

    const char* DeviceTypeStr(DeviceType type) 
    {
        switch (type) {
        case DEVICE_LIGHT:  return "Light";
        case DEVICE_SOUND:  return "Sound";
        case DEVICE_PTZ:    return "PTZ";
        case DEVICE_RELAY:  return "Relay";
        case DEVICE_CAMERA: return "Camera";
        default:            return "Unknown";
        }
    }

    const char* LightModelStr(LightModel model) 
    {
        switch (model) {
        case LIGHT_PT_LD_1307: return "PT-LD-1307";
        case LIGHT_HL_525_4W:  return "HL-525-4W";
        default:               return "Unknown-Light";
        }
    }

    const char* SoundModelStr(SoundModel model) 
    {
        switch (model) {
        case SOUND_NETSPEAKER_V2: return "NetSpeaker-V2";
        default:                  return "Unknown-Sound";
        }
    }

    const char* PTZModelStr(PTZModel model) 
    {
        switch (model) {
        case PTZ_YZ_BY010W: return "YZ-BY010W";
        default:            return "Unknown-PTZ";
        }
    }

    const char* RelayModelStr(RelayModel model)
    {
        switch (model)
        {
        case RELAY_TAS_IO_428R2:    return "RELAY-TAS-IO-428R2";
        default:                    return "Unknown-Relay";
        }
    }
    const char* CameraModelStr(CameraModel model)
    {
        switch (model)
        {
        case CAMERA_HKVISION:   return "CAMERA-HKVISION";
        case CAMERA_HSXVISION:  return "CAMERA-HSXVISION";
        default:                return "Unknown-Camera";
        }
    }

}

// --- DeviceID Implementation ---

str DeviceID::ToHexString() const {
    char buf[16] = { 0 };
    // 输出格式：0x01000100 (Type 01, Model 0001, Index 00)
    snprintf(buf, sizeof(buf), "0x%08X", m_id);
    return str(buf);
}

str DeviceID::ToString() const {
    did::DeviceType type = GetDeviceType();
    u16 model = GetModel();
    u8 idx = GetIndex();

    str catStr = did::DeviceTypeStr(type);
    str modelStr;

    switch (type) {
    case did::DEVICE_LIGHT:     modelStr = did::LightModelStr((did::LightModel)model);      break;
    case did::DEVICE_SOUND:     modelStr = did::SoundModelStr((did::SoundModel)model);      break;
    case did::DEVICE_PTZ:       modelStr = did::PTZModelStr((did::PTZModel)model);          break;
    case did::DEVICE_RELAY:     modelStr = did::RelayModelStr((did::RelayModel)model);      break;
    case did::DEVICE_CAMERA:    modelStr = did::CameraModelStr((did::CameraModel)model);    break;
    default:                    modelStr = "UnknownModel";                                  break;
    }

    char buf[128] = { 0 };
    // 格式: Light/HL-525-4W[0]
    snprintf(buf, sizeof(buf), "%s/%s[%d]", catStr.c_str(), modelStr.c_str(), idx);
    return str(buf);
}

ECCS_END