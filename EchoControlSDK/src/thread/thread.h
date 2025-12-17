
#pragma once
#include "event_queue.h"

ECCS_BEGIN


class Thread
{
    NON_COPYABLE(Thread);

public:
    Thread();
    virtual ~Thread();

    typedef ECCS_C11 thread::id tid;
    tid threadId() const;
    bool isRunning() const;
    bool isQuitted() const;
    bool isStopped() const;

public:
    void start();
    void quit();
    void join();

    virtual void postEvent(Event* e);  // take ownership

protected:
    Event_Ptr peekEvent();
    Event_Ptr popEvent();

    void innerRun();
    virtual void run() = 0;
    virtual bool onEvent(Event_Ptr& e);
    virtual bool processEvent(bool block = true);

protected:
    enum ThreadState
    {
        TS_UNINIT,
        TS_RUNNING,
        TS_QUIT,
        TS_STOPPED,
    };
    typedef std::shared_ptr<ECCS_C11 thread> Thread_Ptr;

    ThreadState  m_state;
    EventQueue   m_eq;
    Thread_Ptr   m_thread;
};


ECCS_END
