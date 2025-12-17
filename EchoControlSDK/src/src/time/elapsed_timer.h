
#pragma once
#include "../global.h"
#include "sal_chrono.h"

ECCS_BEGIN


//------------------------------------------------------
// ElapsedTimer
//------------------------------------------------------

class ElapsedTimer
{
public:
    ElapsedTimer();

    void start();
    i64 restart();        // ms
    i64 elapsed() const;  // ms
    i64 uelapsed() const; // us

private:
    steady_clock::time_point _tp;
};


//------------------------------------------------------
// Timer
//------------------------------------------------------

class Timer
{
public:
    Timer();

    bool isRunning() const;
    i64 elapsed() const;  // ms
    i64 uelapsed() const; // us

    void reset();
    void start();
    void stop();

private:
    bool  _running;
    i64   _time;    // us
    ElapsedTimer _et;
};


ECCS_END
