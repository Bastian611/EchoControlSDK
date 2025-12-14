
#include "elapsed_timer.h"
#include <time.h>

ECCS_BEGIN


//------------------------------------------------------
// ElapsedTimer
//------------------------------------------------------

ElapsedTimer::ElapsedTimer()
{
    start();
}

void ElapsedTimer::start()
{
    _tp = steady_clock::now();
}
i64 ElapsedTimer::restart()
{
    i64 et = elapsed();
    _tp = steady_clock::now();
    return et;
}
i64 ElapsedTimer::elapsed() const
{
    auto tp = steady_clock::now();
    auto et = ECCS_C11 chrono::duration_cast<duration_ms>(tp - _tp).count();
    return (i64)et;
}
i64 ElapsedTimer::uelapsed() const
{
    auto tp = steady_clock::now();
    auto et = ECCS_C11 chrono::duration_cast<duration_us>(tp - _tp).count();
    return (i64)et;
}


//------------------------------------------------------
// Timer
//------------------------------------------------------

Timer::Timer() : _running(false), _time(0)
{

}

bool Timer::isRunning() const
{
    return _running;
}
i64 Timer::elapsed() const
{
    return _running ? _et.elapsed() : _time/1000;
}
i64 Timer::uelapsed() const
{
    return _running ? _et.uelapsed() : _time;
}

void Timer::reset()
{
    _time = 0;
    _running = false;
}
void Timer::start()
{
    _time = 0;
    _et.start();
    _running = true;
}
void Timer::stop()
{
    if (_running){
        _time = _et.uelapsed();
        _running = false;
    }
}


ECCS_END
