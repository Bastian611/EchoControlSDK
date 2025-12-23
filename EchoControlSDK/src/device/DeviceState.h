#pragma once
#include "../global.h"

ECCS_BEGIN

enum DevState : u8 {
    STATE_UNKNOWN       = 0,
    STATE_INITIALIZED   = 1,
    STATE_OFFLINE       = 2, // 离线/断连
    STATE_CONNECTING    = 3, // 连接中
    STATE_ONLINE        = 4, // 在线/空闲
    STATE_WORKING       = 5, // 工作中 (如正在播放、正在转动)
    STATE_ERROR         = 6  // 故障
};

// 状态转字符串 (方便日志)
inline const char* DevStateToStr(DevState s) {
    switch (s) {
    case DevState::STATE_OFFLINE:       return "Offline";
    case DevState::STATE_INITIALIZED:   return "Initialized";
    case DevState::STATE_CONNECTING:    return "Connecting";
    case DevState::STATE_ONLINE:        return "Online";
    case DevState::STATE_WORKING:       return "Working";
    case DevState::STATE_ERROR:         return "Error";
    default: return "Unknown";
    }
}

ECCS_END