
#pragma once
#include <map>
#include "../global.h"

ECCS_BEGIN


//------------------------------------------------------
// CreateMap可用于std::map对象初始化
// * 主要为了应对编译器不支持C++11的初始化列表，而需要初始化
//   静态std::map对象的情况
//
// 使用用法：
//   std::map<int, char> m = CreateMap<int, char>(1, 'a')(2, 'b');
//------------------------------------------------------

template <typename T, typename U>
class CreateMap
{
public:
    CreateMap(const T& key, const U& val)
    {
        _map[key] = val;
    }

    CreateMap<T, U>& operator()(const T& key, const U& val)
    {
        _map[key] = val;
        return *this;
    }
    operator std::map<T, U>()
    {
        return _map;
    }

private:
    std::map<T, U> _map;
};


ECCS_END
