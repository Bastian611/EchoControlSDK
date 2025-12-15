
#include "Logger.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <algorithm>
#include "../global.h"
#include "../time/time_utils.h"
#include "../utils/utils.h"
#include "../utils/file_system.h"
#define _INVALID_FILE_     -1
#define _FPRINTFS_FAILED_  -2

ECCS_BEGIN


Logger::Logger()
{
    m_time_zone =8.0;
    m_fout = NULL;
    m_fout_operate = NULL;
    m_fout_record = NULL;
    m_time_from_start_log = 0;
    log_out=true;
    custom_buf.clear();
    is_write_custom=false;
    custom_log_path="";
}
Logger::~Logger()
{
    if (m_fout){
        fclose(m_fout);
        m_fout = NULL;
    }
    if (m_fout_operate){
        fclose(m_fout_operate);
        m_fout_operate = NULL;
    }
    if(m_fout_record)
    {
        fclose(m_fout_record);
        m_fout_record = NULL;
    }
}
Logger* Logger::getInstance()
{
    static Logger logger;
    return &logger;
}
void Logger::setTimeZone(float tz)
{
    m_time_zone = tz;
}

void Logger::setLogOut(bool logout)
{
    log_out = logout;
}


/**
 * @brief Logger::isOpened 判断log文件是否打开
 * @return true:log文件打开 false：log文件未打开
 */
bool Logger::isOpened() const
{
    return (m_fout != NULL);
}
/**
 * @brief Logger::elapsedFromLastLog 获取距离上次log记录的时间
 * @return 距离上次log记录的时间间隔
 */
i64 Logger::elapsedFromLastLog() const
{
    return m_lastLog.elapsed();
}
i64 Logger::elapsedFromStartLog()
{
    SMART_LOCK(m_lock);
    return m_time_from_start_log;
}

void Logger::logCustom(const char* fmt, ...){
    va_list args;
    va_start(args, fmt);
    char cmdStr[255];
    memset(cmdStr,0,sizeof(cmdStr));
    snprintf(cmdStr, sizeof(cmdStr), fmt, args);
    va_end(args);
    str ss=str(cmdStr);

    if(is_write_custom){
        std::ofstream ofs;
        ofs.open(custom_log_path.c_str(),std::ios::app);
        if (!ofs.is_open()){
            return ;
        }
        ofs.write(ss.c_str(),ss.length());
        ofs.close();
    }else{
       custom_buf.push_back(ss.c_str(),ss.length());
    }
}

void Logger::setCustomPath(const char* path){
    custom_log_path=str(path);
}
bool Logger::writeCustom(bool mount){
    if(custom_log_path.length()<5){
        return false;
    }
    if(is_write_custom){
        return false;
    }
    if(isFileDirectoryExisted(custom_log_path.c_str())){
        deleteDirectory(custom_log_path.c_str());
    }
    if (!isFileDirectoryExisted(custom_log_path.c_str())){
        createDirectory(custom_log_path.c_str());
    }
    custom_log_path=custom_log_path+"/message.txt";


    std::ofstream ofs;
    ofs.open(custom_log_path.c_str(),std::ios::binary);
    if (!ofs.is_open()){
        return false;
    }
    ofs.write(custom_buf.buf(),custom_buf.size());
    ofs.close();

//    FILE* file=fopen(custom_log_path.c_str(),"w+");
//    fwrite(custom_buf.buf(),custom_buf.size());
//    fprintf(file,"%s",custom_buf.buf());
//    fflush(file);
//    fclose(file);
//    custom_log_path="";
    custom_buf.clear();
    is_write_custom=true;
}

void Logger::backupLogs()
{
    if("" == m_backup_path_str)
    {
        return;
    }
    if("" == m_log_path_str)
    {
        return;
    }
    char cmdStr[255];
    char cmdOut[255];
    memset(cmdOut,0,sizeof(cmdOut));
    memset(cmdStr,0,sizeof(cmdStr));
    snprintf(cmdStr, sizeof(cmdStr), "chmod -R 777 %s*",m_backup_path_str.c_str());
    mySystem(cmdStr);
    snprintf(cmdStr, sizeof(cmdStr), "rm -r %s*",m_backup_path_str.c_str());
    mySystem(cmdStr);
    if(m_backup_path_str.find_last_of("/")==m_backup_path_str.length()-1){
        str backDir=m_backup_path_str.substr(0,m_backup_path_str.length()-1);
        LOG_INFO("backDir=%s m_backup_path_str=%s",backDir.c_str(),m_backup_path_str.c_str());
        createDirectory(backDir.c_str());
    }
    SMART_LOCK(m_lock);
    //判断当前剩余存储空间是否足够,如果不够则删除日志至空间足够
    i64 emmc_space_free_byte = diskFree("/");
    std::cout << "emmc_space_free_byte = " << emmc_space_free_byte << std::endl;
    std::vector<str> log_dirs = scandir(m_log_path_str.c_str(), SCAN_DIR, 0, true, "[0-9]+_[0-9]+_[0-9]+");
    //将log_dirs按时间先后排序,最老的在列表最前
    std::vector<int> log_dirs_time;//日志目录名时间,int 如:20220202
    int date_time;
    int year;
    int month;
    int day;
    for (auto log:log_dirs) {
        sscanf(log.c_str(),"%d_%d_%d",&year,&month,&day);
        date_time = year*10000 + month*100 + day;
        log_dirs_time.push_back(date_time);
        printf("date_time = %d\r\n",date_time);
    }
    printf("Before sort:\r\n");
    for(auto x : log_dirs_time)
        std::cout << x << std::endl;
    std::sort(log_dirs_time.begin(),log_dirs_time.end());
    printf("After sort:\r\n");
    for(auto x : log_dirs_time)
        std::cout << x << std::endl;
    while(emmc_space_free_byte < 104857600L/*100M大小*/)
    {
        //开始删除日志文件
        if(0 == log_dirs.size())
        {
            //log目录中已经没有日志文件
            std::cout << "No log files in logs dir" <<std::endl;
            return ;
        }
        char buffer[256];
        int oldest_date = log_dirs_time.front();
        year = oldest_date/10000;
        month = (oldest_date%10000) / 100;
        day = oldest_date%100;
        snprintf(buffer,256,"%s/%04d_%02d_%02d",m_log_path_str.c_str(),year,month,day);

        std::cout << "delete dir : " << buffer <<std::endl;

        int ret = deleteDirectory(buffer);

        log_dirs_time.erase(log_dirs_time.begin());
        emmc_space_free_byte = diskFree("/");
        std::cout <<"ret = " << ret <<" emmc_space_free_byte = "  <<emmc_space_free_byte << std::endl;
    }

    char logs_file_name[256];
    //当前日志加密压缩后存放至硬盘SSD/DATA中
    //选择最新的日志的日期作为压缩包的时间
    char buffer[256];
    int oldest_date = log_dirs_time.back();
    year = oldest_date/10000;
    month = (oldest_date%10000) / 100;
    day = oldest_date%100;
    snprintf(buffer,256,"%s/%04d_%02d_%02d",m_log_path_str.c_str(),year,month,day);
    std::vector<str> logs = scandir(buffer, SCAN_FILE, 0, true, "eccs-service_[0-9]{8}_[0-9]{6}.log");
    std::cout << "logs size = "<<logs.size()<<std::endl;
    for(auto x : logs)
        std::cout << x << std::endl;

    std::vector<int> log_times;//日志文件名时间中的时分秒字段集合,int 如:092516
    int year_month_day = 0;
    int hour_min_sec = 0;
    if(logs.size() > 0)
    {
        for (auto log:logs) {
            sscanf(log.c_str(),"eccs-service_%08d_%06d",&year_month_day,&hour_min_sec);
            log_times.push_back(hour_min_sec);
            printf("year_month_day = %d , hour_min_sec = %d\r\n",year_month_day,hour_min_sec);
        }
        //找到最新的时间
        std::sort(log_times.begin(),log_times.end());
        std::cout << "The Newest Time is : "<< year_month_day << "_" << log_times.back() << std::endl;

        snprintf(logs_file_name,256,"%sLog_%08d_%06d%s",m_backup_path_str.c_str(),year_month_day,log_times.back(),LOGS_BAKEUP_SUFFIX);
        snprintf(cmdStr, sizeof(cmdStr), "tar cz %s|openssl enc -aes-256-cbc -k eccseccs -out %s",m_log_path_str.c_str(),logs_file_name);
        mySystem(cmdStr,true,2,cmdOut);
    }
    else {
        snprintf(logs_file_name,256,"%sLog_%08d_%06d%s",m_backup_path_str.c_str(),year_month_day,hour_min_sec,LOGS_BAKEUP_SUFFIX);
        snprintf(cmdStr, sizeof(cmdStr), "tar cz %s|openssl enc -aes-256-cbc -k eccseccs -out %s",m_log_path_str.c_str(),logs_file_name);
        mySystem(cmdStr,true,2,cmdOut);
    }

}

/**
 * @brief Logger::initLogger 初始化Logger
 * @param log_path 日志保存目录
 * @param prefix 主日志名前缀
 * @param prefix_for_operate 副日志名前缀
 */
void Logger::initLogger(const char* log_path,const char* backup_path,const char* prefix,bool have_sublog,const char* prefix_for_operate)
{
    if(!log_out){
        return;
    }
    SMART_LOCK(m_lock);

    if(NULL != backup_path)
    {
        if(isDirectoryExisted(backup_path)){
            LOG_INFO("isDirectoryExisted(backup_path)");
            m_backup_path_str = str(backup_path);
            if (m_backup_path_str.back() != '\\' && m_backup_path_str.back() != '/'){
                m_backup_path_str.append("/");
            }
        }else {
            m_backup_path_str ="";
            LOG_INFO("isDirectoryExisted(backup_path) NOT");
        }

    }
    if(NULL != log_path)
    {
        m_log_path_str= str(log_path);
        if (m_log_path_str.back() != '\\' && m_log_path_str.back() != '/'){
            m_log_path_str.append("/");
        }
    }

#if DISABLE_LOG
    return;
#endif
    // close file if it openned
    if (m_fout){
        fclose(m_fout);
    }
    if (m_fout_operate){
        fclose(m_fout_operate);
    }
    if(m_fout_record)
    {
        fclose(m_fout_record);
    }
    // file name
    char log_path_buf[512] = {0};
    auto dt = now(m_time_zone);
    int iRet = snprintf(log_path_buf, sizeof(log_path_buf), "%s/%04d_%02d_%02d", log_path,
        dt.year, dt.mon, dt.day);
    assert(iRet > 0);
    //check log path is exist
    if(!isDirectoryExisted(log_path_buf))
    {
        iRet = createDirectory(log_path_buf);
        assert(iRet == 0);
    }
    char buf[512] = {0};
    iRet = snprintf(buf, sizeof(buf), "%s/%s_%04d%02d%02d_%02d%02d%02d.log",log_path_buf,prefix,
        dt.year, dt.mon, dt.day, dt.hour, dt.min, dt.sec);
    assert(iRet > 0);

    // create file
    m_fout = fopen(buf, "w+");
    if (m_fout){
        printf("Create Log File :%s Success !\n",buf);
        fprintf(m_fout, "---------- %04d-%02d-%02d %02d:%02d:%02d.%03d ----------\n\n",
            dt.year, dt.mon, dt.day, dt.hour, dt.min, dt.sec, dt.ms);

    }
    else
    {
        printf("Create Log File :%s Failed !\n",buf);
    }
    memset(buf,0,512);
    if(have_sublog)
    {
        iRet = snprintf(buf, sizeof(buf), "%s/%s_%04d%02d%02d_%02d%02d%02d.log", log_path_buf,prefix_for_operate,
            dt.year, dt.mon, dt.day, dt.hour, dt.min, dt.sec);
        assert(iRet > 0);
        m_fout_operate = fopen(buf, "w+");
        if (m_fout_operate){
            fprintf(m_fout_operate, "---------- %04d-%02d-%02d %02d:%02d:%02d.%03d ----------\n\n",
                dt.year, dt.mon, dt.day, dt.hour, dt.min, dt.sec, dt.ms);
        }
        iRet = snprintf(buf, sizeof(buf), "%s/eccs-data_%04d%02d%02d_%02d%02d%02d.log", log_path_buf,
            dt.year, dt.mon, dt.day, dt.hour, dt.min, dt.sec);
        assert(iRet > 0);
        m_fout_record = fopen(buf, "w+");
    }
    m_lastLog.restart();
    m_time_from_start_log = 0;
}
/**
 * @brief Logger::plain 将提供的字符串写入log
 * @param fmt：日志内容
 * @param ...：format格式字符串
 * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
 */
int Logger::plain(const char* fmt, ...)
{
    if (!m_fout) {
        return _INVALID_FILE_;
    }
    SMART_LOCK(m_lock);
    m_time_from_start_log += m_lastLog.elapsed();
    m_lastLog.restart();

    va_list args;
    va_start(args, fmt);
    int iRet = vfprintf(m_fout, fmt, args);
    va_end(args);
    if (iRet < 0){
        return _FPRINTFS_FAILED_;
    }

    fflush(m_fout);
    return iRet;
}
/**
 * @brief Logger::debug 将提供的字符串写入log，并标记为Debug
 * @param fmt：日志内容
 * @param ...：format格式字符串
 * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
 */
int Logger::debug(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int iRet = log('D', fmt, args);
    va_end(args);

    return iRet;
}
/**
 * @brief Logger::info 将提供的字符串写入log，并标记为Info
 * @param fmt：日志内容
 * @param ...：format格式字符串
 * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
 */
int Logger::info(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int iRet = log('I', fmt, args);
    va_end(args);

    return iRet;
}
/**
 * @brief Logger::warning 将提供的字符串写入log，并标记为warning
 * @param fmt：日志内容
 * @param ...：format格式字符串
 * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
 */
int Logger::warning(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int iRet = log('W', fmt, args);
    va_end(args);

    return iRet;
}
/**
 * @brief Logger::error 将提供的字符串写入log，并标记为error
 * @param fmt：日志内容
 * @param ...：format格式字符串
 * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
 */
int Logger::error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int iRet = log('E', fmt, args);
    va_end(args);

    return iRet;
}
/**
 * @brief Logger::operate 将提供的字符串写入log，并标记为operation
 * @param fmt：日志内容
 * @param ...：format格式字符串
 * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
 */
int Logger::operate(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int iRet = log('O', fmt, args);
    va_end(args);

    return iRet;
}
int Logger::recoder(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int iRet = log('R', fmt, args);
    va_end(args);

    return iRet;
}
/**
 * @brief Logger::log
 * @param level:希望记录的日志等级
 * @param fmt：日志内容
 * @param args：format格式字符串
 * @return 失败返回错误码（_INVALID_FILE_ | _FPRINTFS_FAILED_），成功返回写入字节数
 */
int Logger::log(char level, const char* fmt, va_list args)
{
    if(!log_out){
        return 1;
    }
#if DISABLE_LOG
    return 1;
#endif
    if(level == 'O')
    {
        if (!m_fout_operate){
            return _INVALID_FILE_;
        }
    }
    else if(level == 'R')
    {
        if(!m_fout_record)
        {
            return _INVALID_FILE_;
        }
    }
    else
    {
        if (!m_fout){
            return _INVALID_FILE_;
        }
    }


    int iRet = 0, tmpRet = 0;
    SMART_LOCK(m_lock);
    m_time_from_start_log += m_lastLog.elapsed();
    m_lastLog.restart();

    // 生成信息头并写入文件
    char buf[32] = {0};
    auto dt = now(m_time_zone);
    tmpRet = snprintf(buf, sizeof(buf), "[%04d-%02d-%02d %02d:%02d:%02d.%03d] [%c] ",dt.year,dt.mon,dt.day,
        dt.hour, dt.min, dt.sec, dt.ms, level);
    assert(tmpRet == 30);
    if(level == 'O')
    {
        tmpRet = fprintf(m_fout_operate, "%s", buf);
        if (tmpRet < 0){
            return _FPRINTFS_FAILED_;
        }
        iRet += tmpRet;

        // 写入信息
        tmpRet = vfprintf(m_fout_operate, fmt, args);
        if (tmpRet < 0){
            return _FPRINTFS_FAILED_;
        }
        fflush(m_fout_operate);
        iRet += tmpRet;
    }
    else if(level == 'R')
    {
        // 写入信息
        tmpRet = vfprintf(m_fout_record, fmt, args);
        if (tmpRet < 0){
            return _FPRINTFS_FAILED_;
        }
        fflush(m_fout_record);
        iRet += tmpRet;
    }
    else
    {
        tmpRet = fprintf(m_fout, "%s", buf);
        if (tmpRet < 0){
            return _FPRINTFS_FAILED_;
        }
        iRet += tmpRet;

        // 写入信息
        tmpRet = vfprintf(m_fout, fmt, args);
        if (tmpRet < 0){
            return _FPRINTFS_FAILED_;
        }
        fflush(m_fout);
        iRet += tmpRet;
    }

    return iRet;
}


ECCS_END

#undef _INVALID_FILE_
#undef _FPRINTFS_FAILED_
