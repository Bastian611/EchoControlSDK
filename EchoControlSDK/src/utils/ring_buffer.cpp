#include "ring_buffer.h"

ECCS_BEGIN
RingBuffer::RingBuffer(size_t size) : 
    m_buffer(size), m_head(0), m_tail(0), m_capacity(size) 
{

}


size_t RingBuffer::Write(const u8* data, size_t len) 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t written = 0;
    // 简单实现：空间不够就丢弃或只写一部分 (实时流建议丢弃旧数据，这里简化为只写剩余空间)
    size_t available = (m_capacity + m_tail - m_head - 1) % m_capacity;
    if (available < len) len = available;

    for (size_t i = 0; i < len; ++i) {
        m_buffer[m_head] = data[i];
        m_head = (m_head + 1) % m_capacity;
    }
    return len;
}

size_t RingBuffer::Read(u8* outData, size_t len) 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t read = 0;
    size_t count = (m_capacity + m_head - m_tail) % m_capacity;
    if (len > count) len = count;

    for (size_t i = 0; i < len; ++i) {
        outData[i] = m_buffer[m_tail];
        m_tail = (m_tail + 1) % m_capacity;
    }
    return len;
}

size_t RingBuffer::Available() 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return (m_capacity + m_head - m_tail) % m_capacity;
}

ECCS_END