#include "../global.h"


ECCS_BEGIN

// =======================
// 播放模式
// =======================
enum SoundStatus
{
    Idle,           // 空闲 / 停止
    Player,         // 播放器（MP3 列表）
    OneKey,         // 一键驱散
    MicBroadcast    // 实时喊话
};

// =======================
// 播放状态事件
// =======================
enum SoundPlayState
{
    Stopped,
    Playing,
    Finished,
    Error
};

// =======================
// 音频文件信息
// =======================
struct SoundFileInfo
{
    int index;
    str name;
    u32 duration;   // 秒
};

ECCS_END