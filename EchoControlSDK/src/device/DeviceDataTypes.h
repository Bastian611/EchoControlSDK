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

// 自定义Json解析器
struct JsonData
{
    std::map<std::string, std::string> jsonKV;
};

// 自定义Json值
struct JsonValue
{
    str strVal;
    std::vector<JsonValue> arrayVal;
    bool isArray{ false };
};

// 音频发送参数控制
struct AudioSpec {
    size_t chunkSize; // 每一包发送的大小
    int intervalMs;   // 网线直连下建议设为很小（如 1-5ms）或 0
};

ECCS_END