/*******************************************************
此文件将定义一些安佳威视静态库公共定义
*******************************************************/

#ifndef __AJLIBDEF__20170222__H__
#define __AJLIBDEF__20170222__H__

typedef struct
{
	LONG FrameType;
	DOUBLE TimeStamp;
}MPEG4_VIDEO_FRAME_EXTRA;

typedef struct
{
	LONG FrameType;
	DOUBLE TimeStamp;
}G711_AUDIO_FRAME_EXTRA;

typedef struct __VIDEO_STREAM_PARAM
{
	char codec[256];
	int width;
	int height;
	int colorbits;
	int framerate;
	int bitrate;
	char vol_data[256];
	int vol_length;
}VIDEO_STREAM_PARAM;



typedef struct  __AUDIO_STREAM_PARAM
{
	char codec[256];
	int samplerate;
	int bitspersample;
	int channels;
	int framerate;
	int bitrate;
}AUDIO_STREAM_PARAM;


//typedef struct  __STREAM_AV_PARAM
//{
//	unsigned char	ProtocolName[32];
//	short  bHaveVideo;//0 表示没有视频参数
//	short  bHaveAudio;//0 表示没有音频参数
//	VIDEO_PARAM videoParam;//视频参数
//	AUDIO_PARAM audioParam;//音频参数
//	char		szUrlInfo[512];//注意长度
//}STREAM_AV_PARAM;



enum AJPLAYER_ERROR_CODE
{
	ERR_PLY_INIT_ERROR = -999999,																	//Lib库初始化错误
	ERR_PLY_CHANNELID_ERROR,																		//播放器句柄错误。
	ERR_PLY_MEDIAPARAM_ERROR,																	//媒体参数错误
	ERR_PLY_DECODEPARAM_ERROR,																//解码参数错误
	ERR_PLY_MEDIAPARAMCHANGED_ERROR,											//媒体参数已改变
	ERR_PLY_DECODERINITFAILED_ERROR,													//解码器初始化失败
	ERR_PLY_NOPLAY_ERROR,									
};

enum AJPLAYER_PLAYMODE
{
	PLAYMODE_REALTIME,			//实时模式
	PLAYMODE_SMOOTH,			//流畅模式
};
#endif