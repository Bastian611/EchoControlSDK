
#pragma once
#include <memory>
#include <deque>
#include "../global.h"
#include "semaphore.h"

ECCS_BEGIN

// 事件类型
namespace EventTypes {
const int
    Quit = 0,
    User = 512;
}
class EventType
{
public:
    static const int Quit=0,
    Test=1,
    Max=511,
    EventUser=512;
};

//------------------------------------------------------
// Event
//------------------------------------------------------

class Event
{
public:
    Event(int id) : m_id(id) { }
    Event(const Event& e) : m_id(e.m_id) { }
    Event& operator=(const Event& e);
    virtual ~Event() { }

    int eId() const
    {
        return m_id;
    }
    bool operator==(int id)
    {
        return (m_id == id);
    }

private:
    int  m_id;
};
typedef std::shared_ptr<Event> Event_Ptr;


//------------------------------------------------------
// EventTemplate
//------------------------------------------------------

template<int Eid>
class EventTemplate : public Event
{
public:
    const static int _EID_ = Eid;
    EventTemplate() : Event(Eid) { }
};


template<int Eid, typename TData>
class EventTemplateEx : public Event
{
public:
    const static int _EID_ = Eid;
    EventTemplateEx() : Event(Eid) { }
    EventTemplateEx(const TData& dat) : Event(Eid), Dat(dat) { }

public:
    TData  Dat;
};


//------------------------------------------------------
// EventQueue
//------------------------------------------------------

class EventQueue
{
    NON_COPYABLE(EventQueue);

public:
    EventQueue();
    virtual ~EventQueue();

public:
    void post(int eid);
    void post(Event_Ptr& e);
    Event_Ptr peek();
    Event_Ptr pop();

    size_t size() const;
    size_t maxSize() const;

protected:
    size_t                  m_maxSize;
    Semaphore               m_sem;
    ECCS_C11 recursive_mutex m_queueLcok;
    std::deque<Event_Ptr>   m_qEvents;
};


ECCS_END
