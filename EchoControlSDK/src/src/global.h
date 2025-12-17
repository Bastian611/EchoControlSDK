
#pragma once

#if defined(ECCS_BEGIN) || defined(ECCS_END) || defined(ECCS) || defined(USING_ECCS)
#error namespace conflicts
#endif

#define ECCS_BEGIN namespace EchoControlSDK {
#define ECCS_END }
#define ECCS ::EchoControlSDK::
#define USING_ECCS using namespace EchoControlSDK;


// Windows: x86_x64 _WIN32 & _WIN64 is defined. x86: _WIN32 is defined only
#if !defined(_WIN32) && !defined(__linux__)
#error platform unsupported
#endif


#if defined(UNICODE) || defined(_UNICODE)
#error unicode unsupported
#endif


#if defined(_MSC_VER)
#  if (_MSC_VER >= 1800)    // >= vc140
#    define SUPPORT_C11 1
#  elif (_MSC_VER != 1600)  // = vc100
#    error compiler unsupported
#  endif
#else
#  if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9))
#    define SUPPORT_C11 1
#  else
#    error compiler unsupported
#  endif
#endif


#ifdef SUPPORT_C11
#ifndef ECCS_C11
#  define ECCS_C11 ::std::
#endif
#else
#  define ECCS_C11 ::boost::
#endif


#define UNUSED(arg) (arg)


#ifdef SUPPORT_C11
#  define NON_COPYABLE(classname) \
       classname(const classname &) = delete; \
       classname & operator=(const classname &) = delete;
#else
#  define NON_COPYABLE(classname) \
       classname(const classname &); \
       classname & operator=(const classname &);
#endif
//是否关闭日志记录功能： 1：关闭，0：打开
#define DISABLE_LOG 0
//用来控制代码是否打印线程资源相关信息的DEBUG日志: 1/开启 0/关闭
#define THREAD_DEBUG_LOG 0
//用来控制临时调试增加的log信息是否显示
#define TMP_DEBUG_LOG 0

#include "types.h"