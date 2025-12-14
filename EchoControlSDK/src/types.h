
#pragma once
#include <string>
#include <string.h>
#include <stdint.h>
#ifdef __linux__
#define vsnprintf_s vsnprintf
#endif


//------------------------------------------------------
//
// type defines
//
//------------------------------------------------------

typedef ::int8_t        i8;
typedef ::uint8_t       u8;
typedef ::int16_t       i16;
typedef ::uint16_t      u16;
typedef ::int32_t       i32;
typedef ::uint32_t      u32;
typedef ::int64_t       i64;
typedef ::uint64_t      u64;

typedef ::std::string   str;
typedef ::std::wstring  wstr;


#ifdef __linux__

#define PRIzu         "zu"
#define PRIzd         "zd"
#define PRIzx         "zx"
#define PRIzX         "zX"

#define SCNzu         "zu"
#define SCNzd         "zd"
#define SCNzx         "zx"
#define SCNzX         "zX"

#else

#if defined(_WIN32) && !defined(_WIN64)

#define PRIzu         PRIu32
#define PRIzd         PRIi32
#define PRIzx         PRIx32
#define PRIzX         PRIX32

#define SCNzu         SCNu32
#define SCNzd         SCNi32
#define SCNzx         SCNx32
#define SCNzX         SCNX32

#endif

#if defined(_WIN32) && defined(_WIN64)

#define PRIzu         PRIu64
#define PRIzd         PRIi64
#define PRIzx         PRIx64
#define PRIzX         PRIX64

#define SCNzu         SCNu64
#define SCNzd         SCNi64
#define SCNzx         SCNx64
#define SCNzX         SCNX64

#endif

#endif


#ifdef SUPPORT_C11

#include <inttypes.h>

#else

#ifndef snprintf
#define snprintf _snprintf_s
#endif


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Output Format Specifier Macros
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define PRId8        "hhd"
#define PRId16       "hd"
#define PRId32       "d"
#define PRId64       "lld"

#define PRIi8        "hhi"
#define PRIi16       "hi"
#define PRIi32       "i"
#define PRIi64       "lli"

#define PRIo8        "hho"
#define PRIo16       "ho"
#define PRIo32       "o"
#define PRIo64       "llo"

#define PRIu8        "hhu"
#define PRIu16       "hu"
#define PRIu32       "u"
#define PRIu64       "llu"

#define PRIx8        "hhx"
#define PRIx16       "hx"
#define PRIx32       "x"
#define PRIx64       "llx"

#define PRIX8        "hhX"
#define PRIX16       "hX"
#define PRIX32       "X"
#define PRIX64       "llX"


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Input Format Specifier Macros
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define SCNd8        "hhd"
#define SCNd16       "hd"
#define SCNd32       "d"
#define SCNd64       "lld"

#define SCNi8        "hhi"
#define SCNi16       "hi"
#define SCNi32       "i"
#define SCNi64       "lli"

#define SCNo8        "hho"
#define SCNo16       "ho"
#define SCNo32       "o"
#define SCNo64       "llo"

#define SCNu8        "hhu"
#define SCNu16       "hu"
#define SCNu32       "u"
#define SCNu64       "llu"

#define SCNx8        "hhx"
#define SCNx16       "hx"
#define SCNx32       "x"
#define SCNx64       "llx"


#endif
