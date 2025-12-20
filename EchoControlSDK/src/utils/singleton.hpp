
#pragma once
#include "../global.h"
#include "../debug/logger.h"
ECCS_BEGIN


template<typename T>
class Singleton
{
    NON_COPYABLE(Singleton);

protected:
    Singleton() {}
    virtual~Singleton() {}

public:
    static T* getInstance()
    {
        static T instance;
        return &instance;
    }
};


ECCS_END
