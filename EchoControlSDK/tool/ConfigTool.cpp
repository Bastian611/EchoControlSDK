#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>

// --------------------------------------------------------
// ID 定义 (必须与 DeviceID.h 保持一致)
// --------------------------------------------------------
typedef unsigned int u32;

enum DeviceType {
    DEVICE_LIGHT = 1,
    DEVICE_SOUND = 2,
    DEVICE_PTZ = 3,
    DEVICE_RELAY = 4,
    DEVICE_CAMERA = 5
};

enum LightModel { LIGHT_PT_LD_1307 = 1, LIGHT_HL_525_4W = 2 };
enum SoundModel { SOUND_NETSPEAKER_V2 = 1 };
enum PTZModel { PTZ_YZ_BY010W = 1 };

// ID 生成宏: Type(8) | Model(16) | Index(8)
#define MAKE_ID(type, model, idx) (u32)( ((type & 0xFF) << 24) | ((model & 0xFFFF) << 8) | (idx & 0xFF) )

// --------------------------------------------------------
// 生成逻辑
// --------------------------------------------------------

void GenerateDeviceConfig(const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create " << path << std::endl;
        return;
    }

    file << "# EchoControl System Configuration\n\n";

    // 1. 强光设备 (Slot 1)
    // Model: HL-525 (对应 Factory 注册名)
    // ID: 0x01000201 (Light | HL-525 | Index 1)
    u32 id1 = MAKE_ID(DEVICE_LIGHT, LIGHT_HL_525_4W, 1);
    char buf1[32]; snprintf(buf1, 32, "0x%08X", id1);

    file << "[Slot_1]\n";
    file << "Enable=true\n";
    file << "Name=MainLight\n";
    file << "Type=Light\n";
    file << "Model=HL-525\n";
    file << "ID=" << buf1 << "\n"; // 必须匹配，否则SDK校验失败
    file << "IP=192.168.1.100\n";
    file << "Port=8000\n";
    file << "\n";

    // 2. 云台设备 (Slot 2)
    u32 id2 = MAKE_ID(DEVICE_PTZ, PTZ_YZ_BY010W, 1);
    char buf2[32]; snprintf(buf2, 32, "0x%08X", id2);

    file << "[Slot_2]\n";
    file << "Enable=true\n";
    file << "Name=MainPTZ\n";
    file << "Type=PTZ\n";
    file << "Model=YZ-BY010W\n";
    file << "ID=" << buf2 << "\n";
    file << "IP=192.168.1.101\n";
    file << "Port=5000\n";
    file << "\n";

    // 3. 强声设备 (Slot 3)
    u32 id3 = MAKE_ID(DEVICE_SOUND, SOUND_NETSPEAKER_V2, 1);
    char buf3[32]; snprintf(buf3, 32, "0x%08X", id3);

    file << "[Slot_3]\n";
    file << "Enable=true\n";
    file << "Name=MainSpeaker\n";
    file << "Type=Sound\n";
    file << "Model=NetSpeaker-V2\n";
    file << "ID=" << buf3 << "\n";
    file << "IP=192.168.1.102\n";
    file << "Port=9527\n";
    file << "\n";



    file.close();
    std::cout << "Generated: " << path << std::endl;
}

void GenerateGlobalConfig(const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) return;
    file << "[Global]\nVersion=1.0.0\n";
    file.close();
    std::cout << "Generated: " << path << std::endl;
}

int main() {
    // 简单创建文件夹指令 (Windows)
    system("mkdir config 2>nul");

    GenerateGlobalConfig("./config/global.cfg");
    GenerateDeviceConfig("./config/device.cfg");

    std::cout << "Configuration files generated successfully." << std::endl;
    return 0;
}