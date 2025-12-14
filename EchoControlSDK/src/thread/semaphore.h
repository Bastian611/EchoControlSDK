
#pragma once
#include "sal_thread.h"

ECCS_BEGIN


class Semaphore
{
    NON_COPYABLE(Semaphore);

public:
    Semaphore(size_t initval = 0) : _count(initval) { }

    void notify();
    void wait();
    bool try_wait();

    template<class Rep, class Period>
    bool wait_for(const ECCS_C11 chrono::duration<Rep, Period>& d)
    {
        return wait_until(steady_clock::now() + d);
    }

    template< class Clock, class Duration>
    bool wait_until(const ECCS_C11 chrono::time_point<Clock, Duration>& t)
    {
        ECCS_C11 unique_lock<decltype(_mtx)> ul(_mtx);

#ifdef SUPPORT_C11
        if (!_condition.wait_until(ul, t, [&]() { return _count != 0; })) {
            return false;
        }
#else
        while (!_count) {  // !! important !!
            auto cvs = _condition.wait_until(ul, t);
            if (cvs == ECCS_C11 cv_status::timeout) {
                return false;
            }
        }
#endif
        --_count;

        return true;
    }

private:
    size_t                     _count;
    ECCS_C11 mutex              _mtx;
    ECCS_C11 condition_variable _condition;
};


ECCS_END
