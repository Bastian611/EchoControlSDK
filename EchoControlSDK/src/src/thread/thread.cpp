
#include "thread.h"
#include <assert.h>
#include "../debug/Logger.h"
#include  <time.h>
ECCS_BEGIN


Thread::Thread()
{
    m_state = TS_UNINIT;
}
Thread::~Thread()
{
    if (m_state == TS_RUNNING){
        quit();
    }
    join();
}

Thread::tid Thread::threadId() const
{
    return m_thread ? m_thread->get_id() : tid();
}
bool Thread::isRunning() const
{
    return (m_state == TS_RUNNING);
}
bool Thread::isQuitted() const
{
    return (m_state == TS_QUIT);
}
bool Thread::isStopped() const
{
    return (m_state == TS_STOPPED);
}

void Thread::start()
{
    if (m_thread == NULL && m_state == TS_UNINIT){
        m_thread = std::make_shared<ECCS_C11 thread>(&Thread::innerRun, this);
    }
}
void Thread::quit()
{
    if (m_thread != NULL && m_state == TS_RUNNING){
        Event* eQuit = new Event(EventTypes::Quit);
        postEvent(eQuit);
        m_state = TS_QUIT;
    }
}
void Thread::join()
{
    if (m_thread != NULL){
        if (m_thread->joinable()) {
            m_thread->join();
        }
        m_thread.reset();
    }
}

void Thread::innerRun()
{
    m_state = TS_RUNNING;
    run();
    m_state = TS_STOPPED;
}
void Thread::postEvent(Event* e)
{
    if (e != NULL){
        auto ep = std::shared_ptr<Event>(e);
        m_eq.post(ep);
    }
}
Event_Ptr Thread::peekEvent()
{
    return m_eq.peek();
}
Event_Ptr Thread::popEvent()
{
    return m_eq.pop();
}

bool Thread::onEvent(Event_Ptr& e)
{
    assert(e != NULL);

    bool bRet = true;
    switch(e->eId())
    {
    case EventTypes::Quit:
        bRet = false;
        break;
    default:
        break;
    }

    return bRet;
}
bool Thread::processEvent(bool block)
{
    Event_Ptr ep = m_eq.peek();
    if (ep) {
        m_eq.pop();
        return onEvent(ep);
    }

    if (block){
        ep = m_eq.pop();
        if (ep){
            return onEvent(ep);
        }
    }

    return true;
}


ECCS_END
