#include "EchoControlCode.h"
#include "EchoControlSDK.h"
#include <iostream>
#include <thread>
#include <chrono>

//// 简单的回调打印
//void MyCallback(ECCS_HANDLE hDev, int type, const void* data, int len, void* userCtx) {
//    if (type == ECCS_EVT_STATUS_CHANGE) {
//        // 简单打印一下，实际需强转结构体
//        std::cout << "[Callback] Device Status Changed!" << std::endl;
//    }
//    else if (type == ECCS_EVT_PTZ_REPORT) {
//        std::cout << "[Callback] PTZ Angle Reported!" << std::endl;
//    }
//}
//
//int main() {
//    // 初始化 (加载 rules.sys 和 params.dev)
//    if (ECCS_Init() != ECCS_RET_SUCCESS) {
//        std::cerr << "Init Failed!" << std::endl;
//        return -1;
//    }
//    std::cout << "SDK Init Success. Version: " << ECCS_GetVersion() << std::endl;
//
//    // 获取设备数量
//    int count = ECCS_GetDeviceCount();
//    std::cout << "Device Count: " << count << std::endl;
//
//    if (count > 0) {
//        // 获取第一个设备的句柄
//        ECCS_HANDLE hDev = ECCS_GetDeviceByIndex(0);
//        int type = ECCS_GetDeviceType(hDev);
//        std::cout << "Device 0 Type: " << type << std::endl;
//
//        // 注册回调
//        ECCS_RegisterCallback(hDev, MyCallback, nullptr);
//
//        // 根据类型发送指令
//        if (type == ECCS_DEV_LIGHT) {
//            std::cout << "Testing Light..." << std::endl;
//            ECCS_Light_SetSwitch(hDev, 1); // 开灯
//            std::this_thread::sleep_for(std::chrono::seconds(2));
//            ECCS_Light_SetSwitch(hDev, 0); // 关灯
//        }
//        else if (type == ECCS_DEV_PTZ) {
//            std::cout << "Testing PTZ..." << std::endl;
//            ECCS_PTZ_Move(hDev, 1, 30); // 向上转
//            std::this_thread::sleep_for(std::chrono::seconds(2));
//            ECCS_PTZ_Move(hDev, 5, 0);  // 停止
//        }
//    }
//
//    // 释放
//    std::cout << "Releasing..." << std::endl;
//    ECCS_Release();
//    return 0;
//}