
#pragma once
#include "../global.h"
#include "../time/sal_chrono.h"
#ifdef SUPPORT_C11
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <atomic>
#else
#define BOOST_THREAD_PROVIDES_FUTURE
#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/atomic/atomic.hpp>
#endif


#define current_thread() ECCS_C11 this_thread::get_id()
#define sleep_for(d)     ECCS_C11 this_thread::sleep_for(d)
#define sleep_until(t)   ECCS_C11 this_thread::sleep_until(t)

#define SMART_LOCK(l)    ECCS_C11 lock_guard<decltype(l)> lg_##l(l)



inline void deleteThread(ECCS_C11 thread*& t)
{
    if (t) {
        try{
        if (t->joinable()){
            t->join();
        }
        delete t;
        }
        catch(std::exception& e)
        {
            throw e;
        }

        t = NULL;
    }
}
