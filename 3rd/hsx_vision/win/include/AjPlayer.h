#ifndef	__AJPLAYER_20171111_H
#define __AJPLAYER_20171111_H

#include "AjLibDef.h"

#ifdef AJPLAYER_EXPORTS
#define AJPLAYER_API __declspec(dllexport)
#else
#define AJPLAYER_API __declspec(dllimport)
#pragma comment(lib, "AjPlayer.lib")
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	long nWidth;
	long nHeight;
	long nStamp;
	long nType;
	long nFrameRate;
	long bIsVideo;
	int  nLinseSize[4];
}FRAME_INFO;

typedef int (CALLBACK* fDecCallBackFunction)(long nPort,char * pBuf,long nSize,FRAME_INFO * pFrameInfo, void * pUser,long nReserved2);

typedef int (CALLBACK* fQRcodeCallBackFunction)(long nPort,	char* pBuf,void * pUser);

/********************************************************
功能：初始化媒体播放库，在开始使用媒体播放库之前必须且仅调用一次。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_Init();

/********************************************************
功能：释放所有播放器资源，一般在不再需要使用播放器的时候调用。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_ReleaseAll();
/************************************************************************
功能：设置YUV数据回调
************************************************************************/
AJPLAYER_API int 	AJ_PLAYER_SetDecCallBack(LONG nPort,fDecCallBackFunction func,void * pUser);
/********************************************************
功能：初始化播放器窗口。
hWnd：播放器窗口句柄。
返回值：>0表示成功，返回播放器标句柄。	
		<=0 返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_CreatePlayer(HWND hWnd);

/********************************************************
功能：设置解码参数.
nPort：播放器句柄。
isVideo:播放器类型，1--视频、0--音频。
pParam : 媒体参数，
		 当isAudio = 0时
		 请输入typedef struct __VIDEO_STREAM_PARAM
		 {
		 char codec[256];
		 int width;
		 int height;
		 int colorbits;
		 int framerate;
		 int bitrate;
		 char vol_data[256];
		 int vol_length;
		 }VIDEO_PARAM;
		 当isAudio = 1时
		 请输入typedef struct  __AUDIO_STREAM_PARAM
		 {
		 char codec[256];
		 int samplerate;
		 int bitspersample;
		 int channels;
		 int framerate;
		 int bitrate;
		 }AUDIO_PARAM;
pSize：媒体参数的长度。
返回值：0 表示成功，否则返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetupDecoder(int nPort,int isVideo, PBYTE pParam, DWORD pSize);

/*********************************************************
功能：传入媒体数据。
参数：
nPort：播放器句柄。
isVideo：数据类型：0-音频，1-视频。
pBuffer：媒体数据。
nSize：数据大小。
pExtra：额外数据 一般使用下面的结构体
	MPEG4_VIDEO_FRAME_EXTRA
	G711_AUDIO_FRAME_EXTRA
ExtraLength：额外数据长度。
返回值：0 表示成功，否则返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_DecodeFrame(int nPort, int isVideo, BYTE* pBuffer, DWORD nSize, BYTE* pExtra, DWORD ExtraLength);

/*********************************************************
功能：读取正在播放的解码后的视频数据(YV12格式)，只有正在播放时才允许调用
参数：pFrameBuf 取出的数据。
	  pFrameBufSize 数据长度
nPort：播放器句柄。
返回值：0 表示成功，否则返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_ReadOneFrame(int nPort, BYTE *pFrameBuf, LONG *pFrameBufSize);

/*********************************************************
功能：快速清理播放器。
参数：
nPort：播放器句柄。
返回值：0 表示成功，否则返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_ReleaseWithoutWait(int nPort);

/*********************************************************
功能：关闭解码器。
参数：
nPort：播放器句柄。
isVideo：数据类型：0-音频，1-视频
返回值：0 表示成功，否则返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_CloseDecoder(int nPort,int isVideo);

/*********************************************************
功能：摧毁播放器。
参数：
nPort：播放器句柄。
返回值：0 表示成功，否则返回错误码。
注意：要删除播放器 因先调用AJ_PLAYER_CloseDecoder（），建议软件退出时使用AJ_PLAYER_ReleaseAll（）
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_DeletePlayer(int nPort);

/*********************************************************
功能：刷新播放器（）。
参数：
value：1：没有数据输入的时候还会显示最后一帧画面；
	   0：没有数据输入的时候直接显示黑屏。
返回值：0 表示成功，否则返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_RefreshSurface(int value);

/********************************************************
功能：设置播放器播放模式（流畅Or实时模式）.
参数：
nPort：播放器句柄。
nMode：播放模式：0-实时模式；1-流畅模式
返回值：0 表示成功，否则返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetPlayMode(int nPort, int nMode);

/********************************************************
功能：获取播放器播放模式（流畅Or实时模式）.
参数：
nPort：播放器句柄。
返回值：播放模式：0-实时模式；1-流畅模式。
		-1表示失败。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_GetPlayMode(int nPort);

/*********************************************************
功能：播放视频
参数：
nPort：播放器句柄。
Value 0-停止播放，1-开始播放。
返回值：0 表示成功，否则返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetPlayOn( int nPort, LONG Value);

/*********************************************************
功能：播放音频（有且只能开启一个播放器的音频）
参数：
nPort：播放器句柄。
返回值：0 表示成功，否则返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetAudioOn( int nPort);

/*********************************************************
功能：停止音频
参数：
返回值：0 表示成功，否则返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetAudioOff();

/*********************************************************
功能：设置音量
参数：
nPort：播放器句柄。
nVolume: 音量
返回值：0 表示成功，否则返回错误码。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetVolume(int nPort,LONG nVolume);

/*********************************************************
功能：获取音量
参数：
nPort：播放器句柄。
pVolume 音量
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_GetVolume( int nPort,LONG * pVolume);

/*********************************************************
功能：设置高清时是否全屏显示
参数：
nPort：播放器句柄。
bIsFullFill：是否全屏显示。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetFullFillStatus(int nPort,LONG bIsFullFill);

/*********************************************************
功能：取得高清时是否全屏显示
参数：
nPort：播放器句柄。
返回值：0 不是全屏显示。
		1 是全屏显示。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_GetFullFillStatus(int nPort);

/*********************************************************
功能：传入鼠标动作
参数：
nPort：播放器句柄。
返回值：0
注意不要传双击事件
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_InputMouseEvent(LONG nPort,LONG nMsgType,WPARAM wp,LPARAM lp);

/*********************************************************
功能：设置电子放大的状态
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetZoomRectOn(int nPort, LONG nType);

/*********************************************************
功能：获取电子放大的状态
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_GetZoomRectStatus(int nPort);

/*********************************************************
功能：进入移动侦测设置模式
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetMotionDetectConfigOn(int nPort, LONG value);

/*********************************************************
功能：设置移动侦测设置区域状态,
参数：
nPort：播放器句柄。
其中xBlocks为行，
yBlocks为列，
initConfigString里面只能包含0，1字符，且长度必须为xBlocks*yBlocks，1表示选择对应区域，按行优先排列
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetMotionDetectConfig(int nPort, LONG xBlocks, LONG yBlocks, char* pConfigString);

/*********************************************************
功能：取得移动侦测设置区域状态
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_GetMotionDetectConfigString(int nPort, char* pConfigString);

/*********************************************************
功能：//将当前正在播放的图保存成文件。
参数：
nPort：播放器句柄。
FileFormat: 文件格式。
0 YUV数据。
1 jpg图片。
2 BMP图片。
（0-2 sDirName表示目录 自动生成文件名。）
3 JPG图片 sDirName 表示文件名
  
StoragePath （FileFormat 为0-2时）为存储目录，（FileFormat 为3时）为存储文件名。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SnapShot(int nPort, LONG FileFormat, char* sDirName);

/*********************************************************
功能：播放视频
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
//AJPLAYER_API int  AJ_PLAYER_SetAlarmStatus(int nPort, LONG alramCode, LONG duration);

/*********************************************************
功能：设置录像状态
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetRecordStatus(int nPort, LONG value);

/*********************************************************
功能：设置对讲状态
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetTalkStatus(int nPort, LONG value);

/*********************************************************
功能：设置是否要显示状态
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetShowIco(int nPort, LONG value);

/*********************************************************
功能：设置滤镜处理方式
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetFlipType(int nPort, LONG nType);

/*********************************************************
功能：设置Brightness，Contrast
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetContrast(int nPort, LONG nBrightness, LONG nContrast, LONG bEnable);

/*********************************************************
功能：获取Brightness，Contrast
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_GetContrast(int nPort, LONG *nBrightness, LONG *nContrast, LONG *bEnable);

/*********************************************************
功能：设置Gamm
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetGamma(int nPort, LONG nGamma, LONG bEnable);

/*********************************************************
功能：获取Gamm
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_GetGamma(int nPort, LONG* nGamma, LONG* bEnable);

/*********************************************************
功能：设置缓存大小
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetBufferTick(int nPort, LONG  maxMaxSecond);

/*********************************************************
功能：清理缓存
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_ClearBufferTick(int nPort);

/*********************************************************
功能：
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetBufferCount(int nPort, LONG nCount);

/*********************************************************
功能：是否开启二维码扫描
参数：
nPort：播放器句柄。
返回值：0 表示成功。
*********************************************************/
AJPLAYER_API int  AJ_PLAYER_SetQRcodeScan(int nPort, int bOpen, fQRcodeCallBackFunction func,void* pUser);

/*********************************************************
功能：获取视频中解析的二维码
参数：
nPort：播放器句柄。
返回值：二维码结果。
*********************************************************/
//AJPLAYER_API CString  AJ_PLAYER_GetQRcode(int nPort);

#ifdef __cplusplus
}
#endif
#endif