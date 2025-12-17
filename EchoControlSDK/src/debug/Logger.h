
#pragma once
#include "../global.h"
#include "../time/elapsed_timer.h"
#include "../thread/sal_thread.h"
#include "limits.h"
#include <iostream>
#include <fstream>
#include "../utils/buffer.h"

ECCS_BEGIN


//-----------------------------------------------
// BYTE0:
//   0x00 - disable log
//   0x01 - log to console
//   0x02 - log to file
//   0xFF - log to file & console
// BYTE1:
//   0x01 - add __FUNCTION__ & __LINE__ to file
//   0x02 - add __FUNCTION__ & __LINE__ to console
//   0x04 - disable plain
//   0x08 - disable debug
//   0x10 - disable info
//   0x20 - disable warning
//   0x40 - disable error
//-----------------------------------------------
#define ECCS_LOG    ((0x01 << 8)/*BYTE1*/ | (0xFF)/*BYTE0*/)


class Logger
{
    NON_COPYABLE(Logger);

private:
    Logger();
    ~Logger();

public:
    const char* LOGS_BAKEUP_SUFFIX = ".tar.gz.enc";
    static Logger* getInstance();
    float getTimeZone()const{return m_time_zone;}
    void setTimeZone(float tz);
    void setLogOut(bool logout);
    /**
     * @brief Logger::isOpened 判断log文件是否打开
     * @return true:log文件打开 false：log文件未打开
     */
    bool isOpened() const;
    /**
     * @brief Logger::elapsedFromLastLog 获取距离上次log记录的时间
     * @return 距离上次log记录的时间间隔
     */

    i64 elapsedFromLastLog() const; // ms
    /**
     * @brief elapsedFromStartLog 获取距离开始日志时的时间 单位为ms
     * @return 时间间隔值
     */
    i64 elapsedFromStartLog();//ms
    /**
     * @name:initLogger
     * @brief Logger::initLogger 初始化Logger
     * @param prefix：log文件标志名
     * @param prefix_for_operate 副日志名前缀
     * @param have_sublog : 是否启用副日志
     */
    void initLogger(const char* log_path,const char* backup_path,const char* prefix, bool have_sublog = false,const char* prefix_for_operate = "ECCS-operation");

    //-----------------------------------------------
    // return value:
    //  >0: bytes written to file
    //   0: no bytes written
    //  -1: invalid file handle
    //  -2: fprintf_s failed
    //-----------------------------------------------
    /**
     * @brief Logger::plain 将提供的字符串写入log
     * @param fmt：日志内容
     * @param ...：format格式字符串
     * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
     */
    int plain(const char* fmt, ...);
    /**
     * @brief Logger::debug 将提供的字符串写入log，并标记为Debug
     * @param fmt：日志内容
     * @param ...：format格式字符串
     * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
     */
    int debug(const char* fmt, ...);
    /**
     * @brief Logger::info 将提供的字符串写入log，并标记为Info
     * @param fmt：日志内容
     * @param ...：format格式字符串
     * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
     */
    int info(const char* fmt, ...);
    /**
     * @brief Logger::warning 将提供的字符串写入log，并标记为warning
     * @param fmt：日志内容
     * @param ...：format格式字符串
     * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
     */
    int warning(const char* fmt, ...);
    /**
     * @brief Logger::error 将提供的字符串写入log，并标记为error
     * @param fmt：日志内容
     * @param ...：format格式字符串
     * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
     */
    int error(const char* fmt, ...);
    /**
     * @brief Logger::operate 将提供的字符串写入log，并标记为operation
     * @param fmt：日志内容
     * @param ...：format格式字符串
     * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
     */
    int operate(const char* fmt, ...);
    /**
     * @brief Logger::operate 将提供的字符串写入log，并标记为recoder
     * @param fmt：日志内容
     * @param ...：format格式字符串
     * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
     */
    int recoder(const char* fmt, ...);
    void backupLogs();

    void logCustom(const char* fmt, ...);
    void setCustomPath(const char* path);
    bool writeCustom(bool mount);
private:
    /**
     * @brief Logger::log
     * @param level:希望记录的日志等级
     * @param fmt：日志内容
     * @param args：format格式字符串
     * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
     */
    int log(char level, const char* fmt, va_list args);

private:
    FILE*                    m_fout;//用于记录服务的代码日志，面向研发人员
    FILE*                    m_fout_operate;//用于记录服务的操作日志，内容简要，面向测试/非开发人员
    FILE*                    m_fout_record;//用于记录数据信息,包括设备电压电流状态,部分数据内容等
    ECCS_C11 recursive_mutex  m_lock;
    ElapsedTimer             m_lastLog;
    i64                      m_time_from_start_log;//记录日志系统启动的总时间,单位为毫秒
    float                    m_time_zone;//日志记录时区
    bool                     log_out;

    str                      custom_log_path;//客户LOG存放目录
    Buffer                   custom_buf;     //客户LOG记录缓存
    bool                     is_write_custom;

public:
    str                      m_backup_path_str;//存放压缩日志的目录路径
    str                      m_log_path_str;//当前日志存放路径
    char                     logs_file_name[256];
};


// log to file
#if (ECCS_LOG & 0x0002)
#if (ECCS_LOG & 0x0100)
#define LOG_CUSTOM(fmt,...)         ECCS Logger::getInstance()->logCustom (fmt "\n",##__VA_ARGS__);
#define LOG_FILE_PLAIN(fmt, ...)    ECCS Logger::getInstance()->plain  (fmt, ##__VA_ARGS__)
#define LOG_FILE_DEBUG(fmt, ...)    ECCS Logger::getInstance()->debug  ("%s:%d  " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_FILE_INFO(fmt, ...)     ECCS Logger::getInstance()->info   ("%s:%d  " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_FILE_WARNING(fmt, ...)  ECCS Logger::getInstance()->warning("%s:%d  " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_FILE_ERROR(fmt, ...)    ECCS Logger::getInstance()->error  ("%s:%d  " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_FILE_OPERATION(fmt, ...)ECCS Logger::getInstance()->operate  ("%s:%d  " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_FILE_REPORT(fmt, ...)   ECCS Logger::getInstance()->recoder  (fmt "\n", ##__VA_ARGS__)
#else
#define LOG_FILE_PLAIN(fmt, ...)    ECCS Logger::getInstance()->plain  (fmt, ##__VA_ARGS__)
#define LOG_FILE_DEBUG(fmt, ...)    ECCS Logger::getInstance()->debug  (fmt "\n", ##__VA_ARGS__)
#define LOG_FILE_INFO(fmt, ...)     ECCS Logger::getInstance()->info   (fmt "\n", ##__VA_ARGS__)
#define LOG_FILE_WARNING(fmt, ...)  ECCS Logger::getInstance()->warning(fmt "\n", ##__VA_ARGS__)
#define LOG_FILE_ERROR(fmt, ...)    ECCS Logger::getInstance()->error  (fmt "\n", ##__VA_ARGS__)
#endif
#else
#define LOG_FILE_PLAIN(fmt, ...)
#define LOG_FILE_DEBUG(fmt, ...)
#define LOG_FILE_INFO(fmt, ...)
#define LOG_FILE_WARNING(fmt, ...)
#define LOG_FILE_ERROR(fmt, ...)
#endif


// log to console
#if (ECCS_LOG & 0x0001)
#if (ECCS_LOG & 0x0200)
#define LOG_CONSOLE_PLAIN(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#define LOG_CONSOLE_DEBUG(fmt, ...)    printf("[D] %s:%d  " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_CONSOLE_INFO(fmt, ...)     printf("[I] %s:%d  " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_CONSOLE_WARNING(fmt, ...)  printf("[W] %s:%d  " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_CONSOLE_ERROR(fmt, ...)    printf("[E] %s:%d  " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define LOG_CONSOLE_PLAIN(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#define LOG_CONSOLE_DEBUG(fmt, ...)    printf("[D] " fmt "\n", ##__VA_ARGS__)
#define LOG_CONSOLE_INFO(fmt, ...)     printf("[I] " fmt "\n", ##__VA_ARGS__)
#define LOG_CONSOLE_WARNING(fmt, ...)  printf("[W] " fmt "\n", ##__VA_ARGS__)
#define LOG_CONSOLE_ERROR(fmt, ...)    printf("[E] " fmt "\n", ##__VA_ARGS__)
#endif
#else
#define LOG_CONSOLE_PLAIN(fmt, ...)
#define LOG_CONSOLE_DEBUG(fmt, ...)
#define LOG_CONSOLE_INFO(fmt, ...)
#define LOG_CONSOLE_WARNING(fmt, ...)
#define LOG_CONSOLE_ERROR(fmt, ...)
#endif


// config log dest
#if ((ECCS_LOG & 0x00FF) == 0)     // disabled

#define LOG_PLAIN(fmt, ...)
#define LOG_DEBUG(fmt, ...)
#define LOG_INFO(fmt, ...)
#define LOG_WARNING(fmt, ...)
#define LOG_ERROR(fmt, ...)

#elif ((ECCS_LOG & 0x00FF) == 0x01) // console only

#define LOG_PLAIN      LOG_CONSOLE_PLAIN
#define LOG_DEBUG      LOG_CONSOLE_DEBUG
#define LOG_INFO       LOG_CONSOLE_INFO
#define LOG_WARNING    LOG_CONSOLE_WARNING
#define LOG_ERROR      LOG_CONSOLE_ERROR

#elif ((ECCS_LOG & 0x00FF) == 0x02) // file only

#define LOG_PLAIN      LOG_FILE_PLAIN
#define LOG_DEBUG      LOG_FILE_DEBUG
#define LOG_INFO       LOG_FILE_INFO
#define LOG_WARNING    LOG_FILE_WARNING
#define LOG_ERROR      LOG_FILE_ERROR

#elif ((ECCS_LOG & 0x00FF) == 0xFF) // console & file

#define LOG_PLAIN(fmt, ...)     LOG_CONSOLE_PLAIN(fmt, ##__VA_ARGS__);   LOG_FILE_PLAIN(fmt, ##__VA_ARGS__);
#define LOG_DEBUG(fmt, ...)     LOG_CONSOLE_DEBUG(fmt, ##__VA_ARGS__);   LOG_FILE_DEBUG(fmt, ##__VA_ARGS__);
#define LOG_INFO(fmt, ...)      LOG_CONSOLE_INFO(fmt, ##__VA_ARGS__);    LOG_FILE_INFO(fmt, ##__VA_ARGS__);
#define LOG_WARNING(fmt, ...)   LOG_CONSOLE_WARNING(fmt, ##__VA_ARGS__); LOG_FILE_WARNING(fmt, ##__VA_ARGS__);
#define LOG_ERROR(fmt, ...)     LOG_CONSOLE_ERROR(fmt, ##__VA_ARGS__);   LOG_FILE_ERROR(fmt, ##__VA_ARGS__);

#endif


// log level control

#if (ECCS_LOG & 0x0400) && defined(LOG_PLAIN)    // disable plain
#undef LOG_PLAIN
#define LOG_PLAIN(fmt, ...)
#endif

#if (ECCS_LOG & 0x0800) && defined(LOG_DEBUG)    // disable debug
#undef LOG_DEBUG
#define LOG_DEBUG(fmt, ...)
#endif

#if (ECCS_LOG & 0x1000) && defined(LOG_INFO)     // disable info
#undef LOG_INFO
#define LOG_INFO(fmt, ...)
#endif

#if (ECCS_LOG & 0x2000) && defined(LOG_WARNING)  // disable warning
#undef LOG_WARNING
#define LOG_WARNING(fmt, ...)
#endif

#if (ECCS_LOG & 0x4000) && defined(LOG_ERROR)    // disable error
#undef LOG_ERROR
#define LOG_ERROR(fmt, ...)
#endif


ECCS_END
