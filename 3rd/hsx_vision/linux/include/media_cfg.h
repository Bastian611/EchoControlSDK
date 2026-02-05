#ifndef _____AJ_MEDIA_CFG_H____
#define _____AJ_MEDIA_CFG_H____

#include <time.h>
#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif

#ifndef StreamTimeStamp
typedef double      StreamTimeStamp;
#endif

//move there by XXX 20111220
typedef struct
{
	int hour;
	int minute;
	int sec;
}DayTime;

typedef struct
{
	DayTime startTime;
	DayTime endTime;
}DayTimeSpan;


typedef	enum
{
	CONFIG_LANGUAGE_CN = 0,
	CONFIG_LANGUAGE_HK,
	CONFIG_LANGUAGE_TW,
	CONFIG_LANGUAGE_EN
}CONFIG_LANGUAGE;


typedef enum
{
	TITLE_ADD_NOTHING = 0,
	TITLE_ADD_RESOLUTION,
	TITLE_ADD_BITRATE,
	TITLE_ADD_RESOLUTION_AND_BITRATE
}titleFormatEn;

typedef enum
{
	POSITION_TYPE_BY_FOUR_CORNER = 0,//四角
	POSITION_TYPE_BY_SCALE = 1,	//画面比例(0-100)
	POSITION_TYPE_DISABLE = 2,
}Positiontype;

typedef enum
{
	TYPE_TYPE_BY_TEXT = 0,//文本
	TYPE_TYPE_BY_BMP = 1,	//BMP图片
}Titletype;


typedef	enum
{
	OSD_POINT_LEFT_TOP = 0,
	OSD_POINT_LEFT_BOTTOM,
	OSD_POINT_RIGHT_TOP,
	OSD_POINT_RIGHT_BOTTOM,
	OSD_POINT_HIDE, 
}PositionByCornerEnum;

typedef struct
{
	unsigned short xScale;//X方向位置,基于整个画面的比例(0-100)
	unsigned short yScale;//Y方向位置,基于整个画面的比例(0-100)
}PositionByScaleStruct;

typedef struct
{
	Positiontype type;
	union
	{
		PositionByCornerEnum value1;	
		PositionByScaleStruct value2;
	};
}osdPointEn;

#define MAX_NAME_LEN 32
typedef struct tag_encode_resolution
{
	char res_name[MAX_NAME_LEN];	//分辨率。更改分辨率选择时，其他值需要根据本结构相应更改
	char codec_name[MAX_NAME_LEN];	//编码格式
	int  stream_type; //0: main, 1: sub, 2: third
	int  def_bitrate; //kbps
	int  min_bitrate; //kbps
	int  max_bitrate; //kbps
	int  def_framerate;	//默认帧率
	int  min_framerate;	//最小帧率
	int  max_framerate;	//实际最大帧率(实际生效的帧率可选值只能是这2者之间)
	int  dual_stream; //if stream_type == 0, dual_stream == 0, means disable sub stream
	int  def_config;  //if def_config = 1, use for default config of video encode
	int  max_display_framerate;	//显示最大帧率(帧率可选值只能是这2者之间)
}RESOLUTION_ENTRY;

typedef struct tag_audio_mode
{
	char codec_name[MAX_NAME_LEN];		//编码方式
	int  channels;			//通道数
	int  bitspersample;	//比特率
	int  samplerate;		//采样率
	int  bitrate;			//比特率
	int  def_config;		//是否默认配置
}AUDIO_CODEC_ENTRY;

typedef struct
{
	int lbr_enable;	
	int lbr_style;		//低码率模式:	0: 保持帧率,自动码率	1: 视频质量优先,自动丢帧
	int lbr_bitratemode;//码率控制:	0: 自动 1:手动
	int lbr_bitrate;		//低码率目标值
	int lbr_motionlevel;	//运动级别:	0: 静止 1:运动幅度小 2:运动幅度大
	int lbr_noicelevel;	//噪点级别:	0: 无 1:低 2:高
}LbrControl;


typedef enum
{
	AJ_OVERLAY_STYLE_BLACK_WHITE = 0,		//黑字白底
	AJ_OVERLAY_STYLE_WHITE_BLACK = 1,		//白字黑底
	AJ_OVERLAY_STYLE_TRANSPARENT_BLACKWHITE = 2,	//透明背景，黑字白框
	AJ_OVERLAY_STYLE_TRANSPARENT_WHITEBLACK = 3,	//透明背景，白字黑框
	AJ_OVERLAY_STYLE_TRANSPARENT_BLACK = 4,	//透明背景，黑字
	AJ_OVERLAY_STYLE_TRANSPARENT_WHITE = 5, //透明背景，白字
}AjOsdOverlayStyle;

typedef enum {
	ALARM_CODE_BEGIN=0,
	ALARM_CODE_LINKDOWN=1,
	ALARM_CODE_LINKUP,
	ALARM_CODE_USB_PLUG,
	ALARM_CODE_USB_UNPLUG,
	ALARM_CODE_SD0_PLUG,
	ALARM_CODE_SD0_UNPLUG,
	ALARM_CODE_SD1_PLUG,
	ALARM_CODE_SD1_UNPLUG,
	ALARM_CODE_USB_FREESPACE_LOW,
	ALARM_CODE_SD0_FREESPACE_LOW,
	ALARM_CODE_SD1_FREESPACE_LOW,
	ALARM_CODE_VIDEO_LOST,
	ALARM_CODE_VIDEO_COVERD,
	ALARM_CODE_MOTION_DETECT,
	ALARM_CODE_GPIO3_HIGH2LOW,	//仅仅用于告警触发判断。IO报警使用ALARM_CODE_IO_ALARM和ALARM_CODE_IO_ALARM_FINISH
	ALARM_CODE_GPIO3_LOW2HIGH,	//仅仅用于告警触发判断。IO报警使用ALARM_CODE_IO_ALARM和ALARM_CODE_IO_ALARM_FINISH
	ALARM_CODE_STORAGE_FREESPACE_LOW, 
	ALARM_CODE_RECORD_START,
	ALARM_CODE_RECORD_FINISHED,	
	ALARM_CODE_RECORD_FAILED,	
	ALARM_CODE_VIDEO_PD,	//人形检测
	ALARM_CODE_VIDEO_PD_FINISH, //人形告警检测去除
	ALARM_CODE_JPEG_CAPTURED,	
	ALARM_CODE_RS485_DATA,		
	ALARM_CODE_SAME_IP,			
	ALARM_CODE_HW130_PIR,
	ALARM_CODE_LPR,	//车牌识别
	ALARM_CODE_AUDIO_BABYCRY,//婴儿啼哭
	ALARM_CODE_AUDIO_LSA,//高分贝声音

	ALARM_CODE_VIDEO_FORMAT_CHANGED,	//格式/分辨率更改，用于通知客户端重新配置解码器

	ALARM_CODE_VIDEO_GATE,//电子围栏

	ALARM_CODE_RESET_TO_FACTORY,//恢复出厂通知
	ALARM_CODE_MOTION_DETECT_DISAPPEAR,  //移动侦测告警消除
	
	ALARM_CODE_IO_ALARM,	//IO输入报警,用于一直按下的情况下，就一直告警
	ALARM_CODE_IO_ALARM_FINISH,	//IO输入报警结束
	ALARM_CODE_VIDEO_PD1,	//人形检测
	ALARM_CODE_VIDEO_PD_FINISH2, //人形告警检测去除
	ALARM_CODE_VIDEO_GATE_FINISH,

	ALARM_CODE_CONFIG_CHANGED, //用于通知devsdk/配置更改
	ALARM_CODE_BEGIN_REBOOT,	//准备重启
	ALARM_CODE_TEMP_HUMID_ALARM, //温湿度告警
	
	ALARM_CODE_EXTERNAL_IO_ALARM,
	ALARM_CODE_EXTERNAL_IO_ALARM_FINISH, 
	ALARM_CODE_CALL, //VOIP 呼叫相关，
	
	ALARM_CODE_END
}AjAlarmCode;

typedef struct
{
	int year;
	int month;
	int day;
	int wday;
	int hour;
	int minute;
	int second;
}ALARM_TIME;

#define MAX_ALARM_DATA_LEN 128

typedef struct 
{
	ALARM_TIME	time;
	AjAlarmCode code;
	int 		flag;
	int			level;
	char 		data[MAX_ALARM_DATA_LEN];
} ALARM_ENTRY;

typedef enum
{
	AUDIO_PLAY_READY,	//预备OK，未播放
	AUDIO_PLAY_RESERVE,//正在播放反向音频
	AUDIO_PLAY_FILE,	//正在播放文件
}AudioPlayStatus;


typedef enum _AudioChannel_e{
	AudioChannel_Mono	= 1,
	AudioChannel_Stereo = 2
}AudioChannel_e;

typedef enum _AudioType_e{
	AudioType_PCMU,
	AudioType_AAC_LC,
	AudioType_PCMA,
	AudioType_PCM,
	AudioType_OPUS,
	AudioType_MP3,
}AudioType_e;

typedef enum _AU_SampleRate_e{
	AU_SampleRate_8000HZ	= 8000,
	AU_SampleRate_16000HZ	= 16000,
	AU_SampleRate_32000HZ	= 32000,
	AU_SampleRate_44100HZ	= 44100,
	AU_SampleRate_48000HZ	= 48000
}AU_SampleRate_e;

typedef struct
{
	int bBlocked;	
	int bRun;
}PcmPlayParam;

//反向音频数据头(新加用于PCM播放)
#define AJ_RA_MAGIC 0xEEbbAAdd
typedef struct
{
	unsigned int magic; //AJ_RA_MAGIC
	unsigned short audiotype;
	unsigned short samplerate;
	unsigned short channels;
	unsigned short reserve;
}RaDataHeader;


#if defined (__cplusplus)
}
#endif

#endif

