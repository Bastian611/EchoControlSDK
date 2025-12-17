
#pragma once
#include "../global.h"

ECCS_BEGIN


//-----------------------------------------------
// 获取系统错误信息
// Windows: strError(GetLastError()); getErrorStr(WSAGetLastError())
// Linux: strError(errno)
//-----------------------------------------------
str strError(i32 error);


ECCS_END
