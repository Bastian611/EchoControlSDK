#include "../include/EchoControlCode.h"
#include "../include/EchoControlSDK.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <atomic>
#include <cstring>

// 全局系统句柄
ECCS_HANDLE g_hSystem = nullptr;
std::atomic<bool> g_simulatingStream(false);
std::thread* g_streamThread = nullptr;

// ---------------------------------------------------------
// 回调函数
// ---------------------------------------------------------
void SystemCallback(ECCS_HANDLE hDev, ECCS_EventType type, const void* data, int len, void* userCtx) {
    // 注意：hDev 这里传回来的也是系统句柄，或者具体设备句柄，取决于SDK实现
    // 但作为用户，我们只关心事件类型
    switch (type) {
    case ECCS_EVT_STATUS_CHANGE:
        printf("\n[Callback] Device Status Changed!\n> ");
        break;
    case ECCS_EVT_PTZ_ANGLE: {
        auto* pos = (ECCS_PtzPosition*)data;
        // 使用 \r 实现原地刷新
        printf("\r[PTZ实时角度] Pan: %.2f, Tilt: %.2f          ", pos->pan, pos->tilt);
        fflush(stdout);
        break;
    }
    case ECCS_EVT_SOUND_FINISH:
        printf("\n[Callback] Sound Playback Finished.\n> ");
        break;
    default:
        break;
    }
}

// ---------------------------------------------------------
// 全双工音频流回传回调 (下行)
// ---------------------------------------------------------
void OnAudioRx(ECCS_HANDLE hDev, const unsigned char* data, int len, void* userCtx) {
    // 实际业务中可将 data 写入 PCM 文件或进行语音播放
}

// ---------------------------------------------------------
// 模拟音频流推送
// ---------------------------------------------------------
void SimulateAudioStream() {
    printf("[Test] Audio Stream Simulation Started.\n");
    char dummyData[1024];
    memset(dummyData, 0x55, sizeof(dummyData));

    while (g_simulatingStream) {
        if (g_hSystem) {
            // 直接推给系统句柄，SDK内部会自动路由给强声模块
            ECCS_Error err = ECCS_Sound_PushData(g_hSystem, dummyData, sizeof(dummyData));
            if (err != ECCS_SUCCESS) {
                // printf("[Test] Push Data Failed: %d\n", err);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    printf("[Test] Audio Stream Simulation Stopped.\n");
}

void PrintHelp() {
    std::cout << "\n==================== EchoControl SDK CLI (V1.0) ====================\n";
    std::cout << " [System 管理]\n";
    std::cout << "  init              : 初始化 SDK 并加载配置文件 (config/device.cfg)\n";
    std::cout << "  release           : 释放资源并停止所有后台工作线程\n";
    std::cout << "  disc              : 手动断开所有底层设备网络连接\n";
    std::cout << "  version           : 获取当前 SDK 内部版本号\n";

    std::cout << "\n [Light 强光控制]\n";
    std::cout << "  l_sw <0/1>        : 强光总开关 (0: 关, 1: 开)\n";
    //std::cout << "  l_mode <1/2/3>    : 切换模式 (1: 不出光, 2: 炫目/绿光, 3: 照明/白光)\n";
    //std::cout << "  l_level <1-10>    : 设置功率等级 (1-10 档)\n";
    std::cout << "  l_strobe <0/1>    : 频闪开关 (注: 必须在强光主开关开启时调用)\n";

    std::cout << "\n [PTZ 云台控制]\n";
    std::cout << "  p_move <1-5> <spd>: 方向控制 (1:上, 2:下, 3:左, 4:右, 5:停). 速度: 1-64\n";
    std::cout << "  p_abs <pan> <tilt>: 精准定位 (Pan: 0-359.9°, Tilt: -90至+90°)\n";
    std::cout << "  p_scan <s1> <s2>  : 设置水平线扫范围 (s1:起始, s2:终止)\n";
    std::cout << "  p_reset           : 云台一键归零并执行硬件重启\n";

    std::cout << "\n [Sound 强声控制]\n";
    std::cout << "  play <idx> <0/1>  : 按索引播放音频. idx: 索引, 0/1: 是否循环\n";
    std::cout << "  stop/next/prev    : 停止播放 / 下一曲 / 上一曲\n";
    std::cout << "  vol <0-100>       : 设置播放器输出音量\n";
    std::cout << "  list              : [同步查询] 获取设备内置音频文件列表 (Rp)\n";
    std::cout << "  mic <0/1>         : 喊话模式开关. (1: 开启并触发测试流推送)\n";

    std::cout << "\n [Ultrasonic 超声控制]\n";
    std::cout << "  u_sw <ch> <0/1>   : 开关控制 (ch: 通道号, 0=所有, 1=通道1...)\n";

    std::cout << "\n [通用]\n";
    std::cout << "  help | quit       : 显示此菜单 | 退出程序\n";
    std::cout << "====================================================================\n";
}

int main() {
    PrintHelp();

    std::string line;
    while (true) {
        std::cout << "\n> ";
        if (!std::getline(std::cin, line))
            break;
        if (line.empty()) 
            continue;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "quit" || cmd == "exit") break;
        if (cmd == "help") { PrintHelp(); continue; }
        if (cmd == "version") { std::cout << "SDK Version: " << ECCS_GetVersion() << std::endl; continue; }

        if (cmd == "init") {
            if (ECCS_Init() == ECCS_SUCCESS) {
                g_hSystem = ECCS_GetHandle();
                ECCS_RegisterCallback(g_hSystem, SystemCallback, nullptr);
                ECCS_Sound_RegisterAudioCallback(g_hSystem, OnAudioRx, nullptr);
                std::cout << "SDK Init Success." << std::endl;
            }
            else {
                std::cout << "SDK Init Failed!" << std::endl;
            }
            continue;
        }

        if (cmd == "release") {
            g_simulatingStream = false;
            if (g_streamThread && g_streamThread->joinable()) g_streamThread->join();
            ECCS_Release();
            g_hSystem = nullptr;
            std::cout << "Resources Released." << std::endl;
            continue;
        }

        if (!g_hSystem) { std::cout << "Error: Please 'init' first." << std::endl; continue; }

        // --- 逻辑分发 (严格匹配 EchoControlSDK.cpp 定义) ---
        if (cmd == "l_sw") {
            int val; 
            ss >> val;
            ECCS_Light_SetSwitch(g_hSystem, val);
        }
        else if (cmd == "l_mode") {
            int val; 
            ss >> val;
            ECCS_Light_SetMode(g_hSystem, val);
        }
        else if (cmd == "l_level") {
            int val; 
            ss >> val;
            ECCS_Light_SetLevel(g_hSystem, val);
        }
        else if (cmd == "l_strobe") {
            int val; 
            ss >> val;
            ECCS_Light_SetStrobe(g_hSystem, val);
        }
        else if (cmd == "p_move") {
            int act, spd; 
            ss >> act >> spd;
            ECCS_PTZ_Move(g_hSystem, act, spd);
        }
        else if (cmd == "p_abs") {
            float p, t; 
            ss >> p >> t;
            ECCS_PTZ_SetAbsolutePos(g_hSystem, p, t);
        }
        else if (cmd == "p_scan") {
            float start, end;
            ss >> start >> end;
            ECCS_PTZ_SetScanRange(g_hSystem, start, end);
        }
        else if (cmd == "p_reset") {
            ECCS_PTZ_Reset(g_hSystem);
        }
        else if (cmd == "p_startscan") {
            ECCS_PTZ_StartScan(g_hSystem);
        }
        else if (cmd == "p_stopscan") {
            ECCS_PTZ_StopScan(g_hSystem);
        }
        else if (cmd == "play") {
            int idx, loop; 
            ss >> idx >> loop;
            ECCS_Sound_Play(g_hSystem, idx, loop);
        }
        else if (cmd == "stop") {
            ECCS_Sound_Stop(g_hSystem);
        }
        else if (cmd == "next") {
            ECCS_Sound_Next(g_hSystem);
        }
        else if (cmd == "prev") {
            ECCS_Sound_Prev(g_hSystem);
        }
        else if (cmd == "vol") {
            int val; 
            ss >> val;
            ECCS_Sound_SetPlayVolume(g_hSystem, val);
        }
        else if (cmd == "getvol") {
            int capVol, playVol;
            ECCS_Sound_QueryPlayVolume(g_hSystem, &playVol, &capVol);
        }
        else if (cmd == "list") {
            ECCS_SoundAudioList audioList;
            //std::cout << "Waiting for Device Sync (500ms timeout)..." << std::endl;
            ECCS_Error err = ECCS_Sound_QueryAudioList(g_hSystem, &audioList);
            if (err == ECCS_SUCCESS) {
                std::cout << "--- Audio List (" << audioList.count << " files) ---" << std::endl;
                for (int i = 0; i < audioList.count; ++i)
                    std::cout << " [" << audioList.files[i].index << "] " << audioList.files[i].name << std::endl;
            }
            else {
                std::cout << "Query Failed: " << ECCS_GetErrorStr(err) << std::endl;
            }
        }
        else if (cmd == "mic") {
            int on; 
            ss >> on;
            ECCS_Sound_SetMic(g_hSystem, on);
            if (on && !g_simulatingStream) {
                g_simulatingStream = true;
                g_streamThread = new std::thread(SimulateAudioStream);
            }
            else if (!on) {
                g_simulatingStream = false;
                if (g_streamThread && g_streamThread->joinable()) {
                    g_streamThread->join(); delete g_streamThread; g_streamThread = nullptr;
                }
            }
        }
        else if (cmd == "u_sw") {
            int ch, on; 
            ss >> ch >> on;
            ECCS_Ultrasonic_SetSwitch(g_hSystem, ch, on);
        }
        else if (cmd == "disc") {
            ECCS_Device_Disconnect(g_hSystem);
            std::cout << "Forced Network Disconnect." << std::endl;
        }
    }

    if (g_hSystem) ECCS_Release();
    return 0;
}