
#include "event_queue.h"

ECCS_BEGIN


Event& Event::operator=(const Event& e)
{
    if (this != &e){
        m_id = e.m_id;
    }
    return (*this);
}

EventQueue::EventQueue()
{
    m_maxSize = m_qEvents.max_size();
}
EventQueue::~EventQueue()
{

}

void EventQueue::post(int eid)
{
    auto ep = std::make_shared<Event>(eid);
    post(ep);
}
void EventQueue::post(Event_Ptr& e)
{
    if (e != NULL && m_qEvents.size() < m_maxSize){
        SMART_LOCK(m_queueLcok);
        m_qEvents.push_back(e);
        m_sem.notify();
    }
}
Event_Ptr EventQueue::peek()
{
    SMART_LOCK(m_queueLcok);
    return m_qEvents.empty() ? NULL : m_qEvents.front();
}
Event_Ptr EventQueue::pop()
{
    m_sem.wait();
    SMART_LOCK(m_queueLcok);
    auto e = m_qEvents.front();
    m_qEvents.pop_front();
    return e;
}
Event_Ptr EventQueue::pop(int timeout_ms)
{
    // 使用信号量的 wait_for
    if (!m_sem.wait_for(ECCS_C11 chrono::milliseconds(timeout_ms))) {
        return nullptr; // 超时返回空
    }

    SMART_LOCK(m_queueLcok);
    if (m_qEvents.empty()) return nullptr;

    auto e = m_qEvents.front();
    m_qEvents.pop_front();
    return e;
}

size_t EventQueue::size() const
{
    return m_qEvents.size();
}
size_t EventQueue::maxSize() const
{
    return m_qEvents.max_size();
}


ECCS_END
