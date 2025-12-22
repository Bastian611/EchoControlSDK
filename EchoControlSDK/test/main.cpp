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
    case ECCS_EVT_PTZ_ANGLE:
        // data 是 PtzPosition 结构体
        printf("\n[Callback] PTZ Angle Reported.\n> ");
        break;
    case ECCS_EVT_SOUND_FINISH:
        printf("\n[Callback] Sound Playback Finished.\n> ");
        break;
    default:
        break;
    }
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
    std::cout << "\n================ EchoControl System CLI ================\n";
    std::cout << " Commands:\n";
    std::cout << "  init              Initialize System\n";
    std::cout << "  release           Release System\n";
    std::cout << "\n  --- Light ---\n";
    std::cout << "  light switch <0/1>   Turn Light Off/On\n";
    std::cout << "  light level <0-100>  Set Brightness\n";
    std::cout << "  light strobe <0/1>   Turn Strobe Off/On\n";
    std::cout << "\n  --- PTZ ---\n";
    std::cout << "  ptz move <act> <spd> Action: 1=Up,2=Down,3=Left,4=Right,5=Stop\n";
    std::cout << "  ptz preset <1/2> <id> Action: 1=Set, 2=Goto\n";
    std::cout << "\n  --- Sound ---\n";
    std::cout << "  sound play <file>    Play audio file\n";
    std::cout << "  sound stop           Stop playback\n";
    std::cout << "  sound vol <0-100>    Set volume\n";
    std::cout << "  sound tts <text>     Text to Speech\n";
    std::cout << "  sound mic <0/1>      Enable/Disable Mic (Starts stream test)\n";
    std::cout << "\n  help              Show this menu\n";
    std::cout << "  quit              Exit\n";
    std::cout << "========================================================\n";
}

int main() {
    PrintHelp();

    std::string line;
    while (true) {
        std::cout << "\n> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "quit" || cmd == "exit") break;
        if (cmd == "help") { PrintHelp(); continue; }

        if (cmd == "init") {
            ECCS_Error err = ECCS_Init();
            if (err == ECCS_SUCCESS) {
                // 获取唯一的系统句柄
                g_hSystem = ECCS_GetHandle();
                if (g_hSystem) {
                    ECCS_RegisterCallback(g_hSystem, SystemCallback, nullptr);
                    std::cout << "Init Success. Version: " << ECCS_GetVersion() << std::endl;
                }
                else {
                    std::cout << "Init Success but Handle is NULL!" << std::endl;
                }
            }
            else {
                std::cout << "Init Failed! Error: " << err << std::endl;
            }
            continue;
        }

        if (cmd == "release") {
            g_simulatingStream = false;
            if (g_streamThread && g_streamThread->joinable()) g_streamThread->join();
            ECCS_Release();
            g_hSystem = nullptr;
            std::cout << "Released." << std::endl;
            continue;
        }

        // 下面的命令都需要句柄
        if (!g_hSystem) {
            std::cout << "Please 'init' first." << std::endl;
            continue;
        }

        // --- 业务指令 ---
        if (cmd == "light") {
            std::string subCmd; ss >> subCmd;
            int val; ss >> val;
            if (subCmd == "switch") ECCS_Light_SetSwitch(g_hSystem, val);
            else if (subCmd == "level") ECCS_Light_SetLevel(g_hSystem, val);
            else if (subCmd == "strobe") ECCS_Light_SetStrobe(g_hSystem, val);
        }
        else if (cmd == "ptz") {
            std::string subCmd; ss >> subCmd;
            if (subCmd == "move") {
                int act, spd; ss >> act >> spd;
                ECCS_PTZ_Move(g_hSystem, act, spd);
            }
            else if (subCmd == "preset") {
                int act, idx; ss >> act >> idx;
                ECCS_PTZ_Preset(g_hSystem, act, idx);
            }
        }
        else if (cmd == "sound") {
            std::string subCmd; ss >> subCmd;
            if (subCmd == "play") {
                std::string file; ss >> file;
                ECCS_Sound_Play(g_hSystem, file.c_str(), 0);
            }
            else if (subCmd == "stop") ECCS_Sound_Stop(g_hSystem);
            else if (subCmd == "vol") {
                int vol; ss >> vol;
                ECCS_Sound_SetVolume(g_hSystem, vol);
            }
            else if (subCmd == "tts") {
                std::string text; std::getline(ss, text);
                ECCS_Sound_TTS(g_hSystem, text.c_str());
            }
            else if (subCmd == "mic") {
                int on; ss >> on;
                ECCS_Sound_SetMic(g_hSystem, on);

                if (on && !g_simulatingStream) {
                    g_simulatingStream = true;
                    g_streamThread = new std::thread(SimulateAudioStream);
                }
                else if (!on) {
                    g_simulatingStream = false;
                    if (g_streamThread && g_streamThread->joinable()) {
                        g_streamThread->join();
                        delete g_streamThread;
                        g_streamThread = nullptr;
                    }
                }
            }
        }
    }

    if (g_hSystem) ECCS_Release();
    return 0;
}