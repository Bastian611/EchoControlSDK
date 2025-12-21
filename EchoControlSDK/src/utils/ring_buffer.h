#pragma once
#include <vector>
#include <mutex>
#include <cstring>
#include "../global.h"

ECCS_BEGIN

class RingBuffer {
public:
    RingBuffer(size_t size);

    // 写入数据
    size_t Write(const u8* data, size_t len);

    // 读取数据
    size_t Read(u8* outData, size_t len);

    size_t Available();

private:
    std::vector<u8> m_buffer;
    size_t m_head, m_tail, m_capacity;
    std::mutex m_mutex;
};

ECCS_END