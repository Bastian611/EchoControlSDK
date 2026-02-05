#ifndef	__AJRTSPCLIENTLIB_20171128_H
#define __AJRTSPCLIENTLIB_20171128_H
//////////////////////////////////////////////////////////////////////////
//由于
//
//
//////////////////////////////////////////////////////////////////////////





#ifdef AJRTSPCLIENTLIB_EXPORTS
#define AJRTSPCLIENTLIB_API __declspec(dllexport)
#else
#ifdef AJRTSPCLIENTLIB_IS_LIB
#define AJRTSPCLIENTLIB_API
#else
#define AJRTSPCLIENTLIB_API __declspec(dllimport)
#pragma comment(lib, "AjRtspClientLib.lib")

#endif
#endif

#ifdef AJRTSPCLIENTLIB_EXPORTS
enum ERROR_CODE
{
	AJ_RTSP_CLIENT_ERR_NOT_INIT				= -800000,
	AJ_RTSP_CLIENT_ERR_INIT_SOCKET_FAILED,
	AJ_RTSP_CLIENT_ERR_INIT_THREAD_FAILED,
	AJ_RTSP_CLIENT_ERR_NOT_FIND_HANDLE,
//	AJ_RTSP_CLIENT_ERR_NOT_FIND_HANDLE,
};

enum STATUS_EVENT_CODE
{
	AJ_RTSP_CLIENT_EVET_CODE = 10000,
	AJ_RTSP_CLIENT_EVET_CODE_RTSP_AUTH_FAILED,
//	AJ_RTSP_CLIENT_EVET_CODE_RTSP_ERR
};

typedef struct
{
	int		bIsKey;
	double	timestamp;
	void    *pUserData;
}FRAME_EXTDATA, *LPFRAME_EXTDATA;

#endif
typedef struct _VIDEO_PARAM
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



typedef struct  _AUDIO_PARAM
{
	char codec[256];
	int samplerate;
	int bitspersample;
	int channels;
	int framerate;
	int bitrate;
}AUDIO_PARAM;


typedef struct  __STREAM_AV_PARAM
{
	unsigned char	ProtocolName[32];
	short  bHaveVideo;//0 表示没有视频参数
	short  bHaveAudio;//0 表示没有音频参数
	VIDEO_PARAM videoParam;//视频参数
	AUDIO_PARAM audioParam;//音频参数
	char		szUrlInfo[512];//注意长度
}STREAM_AV_PARAM;


typedef LONG(CALLBACK *StatusEventCallBack)(LONG lRealHandle, LONG nStateCode, char *pResponse, void *pUser);
typedef LONG(CALLBACK *fRealDataCallBack)(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LPFRAME_EXTDATA  pExtData);





AJRTSPCLIENTLIB_API long __stdcall AJ_RTSP_CLINET_LIB_InitLib();
AJRTSPCLIENTLIB_API long __stdcall AJ_RTSP_CLINET_LIB_ReleaseLib();
AJRTSPCLIENTLIB_API long __stdcall AJ_RTSP_CLINET_LIB_SetStatusEventCallBack(StatusEventCallBack fStatusEventCallBack, void * pUser);
//AJRTSPCLIENTLIB_API long __stdcall AJ_RTSP_CLINET_LIB_SetRealDataCallBack(fRealDataCallBack cbRealDataCallBack, void * dwUser);
AJRTSPCLIENTLIB_API long __stdcall AJ_RTSP_CLINET_LIB_RealPlay(int nVideoChannle, char * pserverip, char *user, char *pass, int nLinkMode, int nVideoPort, fRealDataCallBack cbRealDataCallBack, PVOID pUser);
//连接标准RTSP流
AJRTSPCLIENTLIB_API long __stdcall AJ_RTSP_CLINET_LIB_RealPlay(char * rtspurl, char *user, char *pass, int nLinkMode, fRealDataCallBack cbRealDataCallBack, PVOID pUser);
AJRTSPCLIENTLIB_API long __stdcall AJ_RTSP_CLINET_LIB_RealPlayBySimple(char * pserverip, char *user, char *pass, fRealDataCallBack cbRealDataCallBack, int iStreamType, int  nVideoPort, int  bIsTcp, void *pUserData, BOOL bBlocked);
AJRTSPCLIENTLIB_API long __stdcall AJ_RTSP_CLINET_LIB_StopRealPlay(long lRealHandle);
AJRTSPCLIENTLIB_API long __stdcall AJ_RTSP_CLINET_LIB_StopAllRealPlay();
AJRTSPCLIENTLIB_API long __stdcall AJ_RTSP_CLINET_LIB_GetVideoParam(long  lRealHandle, VIDEO_PARAM *pParam);
AJRTSPCLIENTLIB_API long __stdcall AJ_RTSP_CLINET_LIB_GetAudioParam(long lRealHandle, AUDIO_PARAM *pParam);

#endif