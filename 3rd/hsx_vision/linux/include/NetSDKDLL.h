// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 NETSDKDLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// NETSDKDLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifndef __NETSDKDLL_20110716_H
#define __NETSDKDLL_20110716_H

#include "common_head.h"
#include "media_cfg.h"
#include "data_struct.h"
/*
#ifdef NETSDKDLL_EXPORTS


#if IPCCONFIGDLL
#define NETSDKDLL_API 
#else
#define NETSDKDLL_API __declspec(dllexport)
#endif

#else

#ifdef NETSDKDLL_IS_LIB	//使用lib版本时，全增加以下依赖库
#pragma comment(lib,"libUsageEnvironment.lib")
#pragma comment(lib,"libliveMedia.lib")
#pragma comment(lib,"libgroupsock.lib")
#pragma comment(lib,"libfaad.lib")
#pragma comment(lib,"libfaac.lib")
#pragma comment(lib,"libBasicUsageEnvironment.lib")
#pragma comment(lib,"jthread.lib")
#pragma comment(lib,"jrtplib.lib")
#pragma comment(lib,"NetSDKDLL.lib")
#pragma comment(linker, "/FORCE:MULTIPLE")
#define NETSDKDLL_API
#else
#define NETSDKDLL_API __declspec(dllimport)
#pragma comment(lib, "NetSDKDLL.lib")
#endif
#endif
*/

#define MAX_IPC_SERIALNUMBER	32
#define MAX_DEVICETYPE_LEN 32
#define MAX_TITLE_LEN 256
#define MAX_IPC_P2PID_LEN	48
#define MAX_INTERFACE_NAME_LEN 32

#define RECORD_DISTRIBUTE_LEN	1440// minute of one day
#define REC_TYPE_NO_RECORDING   'C'  // no record in this minute
#define REC_TYPE_UNCONDITIONAL  'A'  // recording, but no event/alarm.
#define REC_TYPE_DRIVEN			'B'  // event/alarm happend when recording

enum REPLAY_IPC_ACTION
{
	ACTION_PLAY=0,
	ACTION_PAUSE,
	ACTION_RESUME,
	ACTION_FAST,
	ACTION_SLOW,
	ACTION_SEEK,
	ACTION_FRAMESKIP,
	ACTION_STOP,
	ACTION_CAPIMG=10,
	ACTION_CHANGE_SOUND,
	ACTION_RECV_DECODEPARAM,
};

typedef struct
{
	char  szInterfaceName[MAX_INTERFACE_NAME_LEN];
	char  ipc_sn[MAX_IPC_SERIALNUMBER];
	char  deviceType[MAX_DEVICETYPE_LEN];
	char  p2p_id[MAX_IPC_P2PID_LEN];
	char  title[MAX_TITLE_LEN];
	UserConfig 	userCfg;
	MediaStreamConfig	streamCfg;
	LANConfig 	lanCfg;
}IPC_ENTRY;


typedef struct _FRAMNE_INFO
{
	int bIsVideo;
	int bIsKeyFrame;
	double TimeStamp;
}FRAMNE_INFO;


#define ALARM_ITEM ALARM_ENTRY

typedef struct
{
	LONG    lChannel;
	LONG    lLinkMode;
	HWND    hPlayWnd;
	char    *sMultiCastIP;
}*LPIP_NET_DVR_CLIENTINFO,IP_NET_DVR_CLIENTINFO;
//LPIP_NET_DVR_CLIENTINFO



#define 	SERIALNO_LEN 48
typedef struct
{
	BYTE     sSerialNumber[SERIALNO_LEN];
	BYTE     byAlarmInPortNum;
	BYTE     byAlarmOutPortNum;
	BYTE     byDiskNum;
	BYTE     byDVRType;
	BYTE     byChanNum;
	BYTE     byStartChan;
}*LPIP_NET_DVR_DEVICEINFO,IP_NET_DVR_DEVICEINFO;


typedef struct 
{

}IP_NET_DVR_ALARMER;


#define MAX_IPADDR_LEN	64
typedef struct
{
	int  nVideoPort;
	int  bIsTcp;
	int  nVideoChannle;
	void *pUserData;
}USRE_VIDEOINFO,* LPUSRE_VIDEOINFO;


enum ERROR_CODE
{
	ERR_NOT_FIND_DEVICE=-9000002,
	ERR_OPEN_AUDIOCAPTURE_FAIL,
	ERR_START_AUDIOCAPTURE_FAIL,
	ERR_AUDIO_PARAM_ERROR,//对讲参数不一致
	ERR_AUDIO_NOT_START,//对讲未启动
	ERR_DEV_NOT_CONNECTED,
	ERR_DEV_NOT_LOGIN,
	ERR_MSGTYPE_ERROR,
	ERR_OUTOFF_MEMORY,
	ERR_INIT_SOCKET_ERROR,
	ERR_PARAM_ERROR,
	ERR_NOT_DEV_EXIST,
	ERR_START_THREADERROR,
	ERR_NOT_FIND_STREAM,
	ERR_ISUPLOADING_ERROR,
	ERR_ISDOWNLOADING_ERROR,
	ERR_IS_STARTAUDIO_ERROR,
	ERR_ISFINISH_ERROR,
	ERR_NOT_DOWNLOAD_MODE_ERROR,
	ERR_PTZCMD_ACTION_ERROR,
	ERR_LOC_FILE_ERROR,
	ERR_NOT_REPLAY_MODE_ERROR,
	ERR_PLAY_ACTION_ERROR,
	ERR_NOT_ALLOW_REPLAY_ERROR,
	ERR_MEMORY_SIZE_ERROR,
	ERR_XML_FORMAT_ERROR,
	ERR_CREATE_SOCKET_ERROR,
	ERR_SEND_MODIFYCMD_ERROR,
	ERR_NOT_STARTTALK_MODE_ERROR,
	ERR_RECORD_MEDIA_PARAM_ERROR,
	ERR_RECORD_CREATEERROR,
	ERR_RECORD_ISRECORDING,
	ERR_RECORD_FILEMAXSECONDS_ERROR,
	ERR_RECORD_ALLRECORDSECONDS_ERROR,
	ERR_RECORD_NOTRUNNING,
	ERR_RECORD_STREAMPARAM_ERROR,
	ERR_RECORD_WRITETEMPBUFFER_ERROR,
	ERR_RECORD_ISNOTRECORDSTREAM_MODE,
	ERR_RECORD_NOTINPUTSTREAM_MODE,
	ERR_RECORD_FILEPATH_ERROR,
	ERR_SEARCH_THREAD_NOT_START,
	ERR_TIME_OUT,

	ERR_MEMORY_ERR = -8000000,
};



enum enumNetSatateEvent
{
	EVENT_CONNECTING,   //0
	EVENT_CONNECTOK,
	EVENT_CONNECTFAILED,
	EVENT_SOCKETERROR,
	EVENT_LOGINOK,
	EVENT_LOGINFAILED,   //5
	EVENT_STARTAUDIOOK,
	EVENT_STARTAUDIOFAILED,
	EVENT_STOPAUDIOOK,
	EVENT_STOPAUDIOFAILED,
	EVENT_SENDPTZOK,    //10
	EVENT_SENDPTZFAILED,
	EVENT_SENDAUXOK,
	EVENT_SENDAUXFAILED,
	EVENT_UPLOADOK,
	EVENT_UPLOADFAILED,  //15
	EVENT_DOWNLOADOK,
	EVENT_DOWNLOADFAILED,
	EVENT_REMOVEOK,
	EVENT_REMOVEFAILED,
	EVENT_SENDPTZERROR,   //20
	EVENT_PTZPRESETINFO,  
	EVNET_PTZNOPRESETINFO,
	EVENT_PTZALARM,
	EVENT_RECVVIDEOPARAM,
	EVENT_RECVAUDIOPARAM,  //25
	EVENT_CONNECTRTSPERROR,
	EVENT_CONNECTRTSPOK,
	EVENT_RTSPTHREADEXIT,
	EVENT_URLERROR,
	EVENT_RECVVIDEOAUDIOPARAM,  //30
	EVENT_LOGIN_USERERROR,
	EVENT_LOGOUT_FINISH, 	//登录线程已停止
	EVENT_LOGIN_RECONNECT, 	//进行重新登录   33
	EVENT_LOGIN_HEARTBEAT_LOST, //心跳丢失
	EVENT_STARTAUDIO_ISBUSY, //  35
	EVENT_STARTAUDIO_PARAMERROR,
	EVENT_STARTAUDIO_AUDIODDISABLED,
	EVENT_CONNECT_RTSPSERVER_ERROR,
	EVENT_CREATE_RTSPCLIENT_ERROR,
	EVENT_GET_RTSP_CMDOPTION_ERROR,     //40
	EVENT_RTSP_AUTHERROR,
	EVNET_RECORD_FILEBEGIN,
	EVENT_RECORD_FILEEND,
	EVENT_RECORD_TASKEND,
	EVENT_RECORD_DISKFREESPACE_TOOLOW,  //45
	EVNET_RECORD_FILEBEGIN_ERROR,
	EVNET_RECORD_WRITE_FILE_ERROR,
	EVENT_RECORD_INITAVIHEAD_ERROR,
	EVENT_RECORD_MEDIA_PARAM_ERROR,	   //49
};








enum PTZ_CMD_TYPE
{
	LIGHT_PWRON=2,
	WIPER_PWRON,
	FAN_PWRON,
	HEATER_PWRON,
	AUX_PWRON1,
	AUX_PWRON2,
	ZOOM_IN_VALUE= 11,
	ZOOM_OUT_VALUE, 
	FOCUS_NEAR, 
	FOCUS_FAR, 
	IRIS_OPEN, 
	IRIS_CLOSE,
	TILT_UP,
	TILT_DOWN,
	PAN_LEFT,
	PAN_RIGHT,
	UP_LEFT,
	UP_RIGHT,
	DOWN_LEFT,
	DOWN_RIGHT,
	PAN_AUTO,
	STOPACTION
};

#ifndef ZOOM_IN	
#define ZOOM_IN ZOOM_IN_VALUE
#define ZOOM_OUT	ZOOM_OUT_VALUE
#endif

enum PTZ_PRESET_TYPE
{
	SET_PRESET= 8 ,
	CLE_PRESET= 9,
	GOTO_PRESET= 39
};


typedef struct 
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


typedef struct 
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
	unsigned char	ProtocolName[32];	//==AV_FALG
	short  bHaveVideo;//0 表示没有视频参数
	short  bHaveAudio;//0 表示没有音频参数
	VIDEO_PARAM videoParam;//视频参数
	AUDIO_PARAM audioParam;//audio param
	char  szUrlInfo[512];
}STREAM_AV_PARAM;


typedef struct __StateEventMsgInfo
{
	char szInfo[1024];
	char szUrlInfo[512];
}STATE_EVENT_MSGINFO;

typedef struct
{
	int bIsKey;
	double	timestamp;
	void    *pUserData;
}FRAME_EXTDATA,* LPFRAME_EXTDATA;

//playback data head info
typedef struct _updpackhead
{
	uint32_t frame_timestamp;
	uint32_t keyframe_timestamp;
	uint16_t pack_seq;
	uint16_t payload_size;
	uint8_t pack_type;//0x01 first packet閿熸枻鎷穢10 last packet, 0x11 first and last packet閿熸枻鎷穢00 middle packet
	uint8_t frame_type;
	uint8_t stream_type;//0: video, 1: audio
	uint8_t stream_index;
	uint32_t  frame_index;

}UpdPackHead;

typedef struct
{
	long 	stream_id;
	VIDEO_PARAM * video_param;
}VIDEO_STATE_MSG_PARAM;


typedef LONG(CALLBACK *MSGCallBack)(LONG lCommand,IP_NET_DVR_ALARMER *pAlarmer,char *pAlarmInfo,DWORD BufLen,void *pUser);
typedef LONG(CALLBACK *StatusEventCallBack)(LONG lUser,LONG nStateCode,char *pResponse,void *pUser);
typedef LONG(CALLBACK *AUXResponseCallBack)(LONG lUser,LONG nType,char *pResponse,void *pUser);


typedef LONG(CALLBACK *fVoiceDataCallBack)(LONG lVoiceComHandle,char *pRecvDataBuffer,DWORD dwBufSize,BYTE byAudioFlag,LPFRAME_EXTDATA  pUser);
typedef LONG(CALLBACK *fRealDataCallBack)(LONG lRealHandle,DWORD dwDataType,BYTE *pBuffer,DWORD dwBufSize,LPFRAME_EXTDATA  pExtData);


typedef LONG(CALLBACK *fPlayActionEventCallBack)(LONG lUser,LONG nType,LONG nFlag,char * pData,void * pUser);


typedef LONG(CALLBACK *fExceptionCallBack)(DWORD dwType,LONG lUserID,LONG lHandle,void *pUser);




enum FILE_TYPE
{
	LOG_FILE,
	RECORD_FILE,
	CONFIG_FILE,
	UPDATE_FILE
};



#ifdef IPCCONFIGDLL
#undef IPCCONFIGDLL	
#define IPCCONFIGDLL
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

	//SDK初始化以及相关信息
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Init();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Cleanup();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetSDKBuildVersion();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Get_Timestamp();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetSDKVersion();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetLogToFile(DWORD bLogEnable,char *strLogDir,BOOL bAutoDel);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_LOG_OPEN();	//开启SDK LOG
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_LOG_CLOSE();//关闭SDK LOG


	//全局callback
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetExceptionCallBack(UINT nMessage,HWND hWnd,fExceptionCallBack cbExceptionCallBack,void *pUser);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetAUXResponseCallBack(AUXResponseCallBack fAUXCallBack,void * pUser);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetStatusEventCallBack(StatusEventCallBack fStatusEventCallBack,void * pUser);


	//设备登陆与连接
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Login(char *sDVRIP,WORD wDVRPort,char *sUserName,char *sPassword,LPIP_NET_DVR_DEVICEINFO lpDeviceInfo);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Logout(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_LogoutAll();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetAutoReconnect(LONG lUserID,int bAutoReconnect);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Reconnect(LONG lUserID);

	//设备搜索功能
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetSearchInterval(UINT nBroadcastInterval);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_StartSearchIPC();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_StopSearchIPC();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetSearchIPCCount();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetIPCInfo(LONG index, IPC_ENTRY * pIPCInfo);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SearchIPCReleaseInfo();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_ModifyIPCByIndex(LONG index, IPC_ENTRY * pIPCInfo);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_ModifyIPCBySN(char *pSN, LANConfig 	*p_lanCfg, MediaStreamConfig* pMediaStreamCfg);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetIPCInfoXML(LONG index, char * pXMLInfo,int maxLen);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_ModifyIPCXML(LONG index, const char * strXML);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetOneIPAddress(char * strResult,int nSize);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetNetworkParam(ULONG nParamIndex, char * strResult,int nSize);

	//实时音视频预览
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_RealPlay(LONG lUserID,LPIP_NET_DVR_CLIENTINFO lpClientInfo,fRealDataCallBack cbRealDataCallBack,LPUSRE_VIDEOINFO pUser,BOOL bBlocked);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_RealPlayEx(char * serverip,char *user,char *pass,fRealDataCallBack cbRealDataCallBack,LPUSRE_VIDEOINFO pUser,BOOL bBlocked);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_StopRealPlay(LONG lRealHandle);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_StopAllRealPlay();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetVideoParam(LONG  lRealHandle,VIDEO_PARAM *pParam);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetAudioParam(LONG lRealHandle,AUDIO_PARAM *pParam);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetRealDataCallBack(fRealDataCallBack cbRealDataCallBack,void * dwUser);


	//云台控制
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_PTZControl(LONG lUser,DWORD dwPTZCommand,DWORD nTspeed,DWORD nSpeed);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_PTZPreset(LONG lUser,DWORD dwPTZPresetCmd,DWORD dwPresetIndex);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_PTZControlEx(LONG lUser, const char *pXml);

	//IPC系统控制
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_FormatDisk(LONG lUserID,LONG lDiskNumber);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetFormatProgress(LONG lFormatHandle,LONG *pCurrentFormatDisk,LONG *pCurrentDiskPos,LONG *pFormatStatic);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Upgrade(LONG lUserID,char *sFileName);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetUpgradeProgress(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetUpgradeState(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_CloseUpgradeHandle(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_FindDVRLogFile(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_RestoreConfig(LONG lUserID);	
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Ircut_switch_daynight(LONG lUserID, int day);//手动切换IRCUT，需要先设置IRCUT模式为MANUAL模式
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetConfigFile(LONG lUserID,char *sFileName);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetConfigFile(LONG lUserID,char *sFileName);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_RebootDVR(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_ShutDownDVR(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetDVRConfig(LONG lUserID,DWORD dwCommand,LONG lChannel,LPVOID lpOutBuffer,DWORD dwOutBufferSize,LPDWORD lpBytesReturned);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SystemControl(LONG lUserID,DWORD nCmdValue,LONG flag, const char * pXml);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetDVRConfig(LONG lUserID,DWORD dwCommand,LONG lChannel,const LPVOID pXml,DWORD dwInBufferSize);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetDeviceAbility(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_WriteAUXStringEx(LONG lUserID,char * pMsgType,LONG nCode,LONG flag,const char * pXml);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_CreateIFrame(ULONG lUserId,int bIsSubStream);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetUserData(ULONG lUserId,char * pOutBuffer,int* nInOutLen);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetUserData(ULONG lUserId,char * pBuffer,int len);


	//文件上传下载
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetFileByName(LONG lUserID,LONG nFileType,char *sDVRFileName,char *saveDir);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_StopGetFile(LONG lFileHandle);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetDownloadState(LONG lFileHandle);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetDownloadPos(LONG lFileHandle);

	//IPC录像文件回放
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetReplayAblity(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetReplayDistribute(LONG lUserID, int year, int month, int day);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetReplayDataCallBack(fRealDataCallBack cbReplayDataCallBack,void * dwUser);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetReplayEventCallBack(fPlayActionEventCallBack cbActionCallback,void * dwUser);
	//NETSDKDLL_API LONG __stdcall IP_NET_DVR_PlayDeviceFile(LONG lUserID,char * filenme);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_ReplayByTime(LONG lUserID, LONG timestampInMillis);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_ControlReplay(LONG lUserID,LONG Action,LONG param);

	//广播音频相关(对讲)
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_StartTalk(int audiotype, int samplerate, int bitspersample, int channels);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_StopTalk();
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_AddTalk(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_RemoveTalk(LONG lUserID);



    NETSDKDLL_API LONG __stdcall IP_NET_DVR_StartVoiceCom(LONG lUserID, int AudioType, int iSampleRate, int iChannel);
    NETSDKDLL_API LONG __stdcall IP_NET_DVR_SetVoiceComClientVolume(LONG lVoiceComHandle,WORD wVolume);
    NETSDKDLL_API LONG __stdcall IP_NET_DVR_StopVoiceCom(LONG lUserID);
    NETSDKDLL_API LONG __stdcall IP_NET_DVR_InputAudioData(LONG lUserID, const char* pBuffer, int nSize);


	//录像相关
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_StartRecord(LONG lRealHandle,const char * filePath,int nFileMaxSeconds,int nAllRecordMaxSeconds);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_StopRecord(LONG lRealHandle);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetLastErrorCode(int nType);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_StartRecordStream(STREAM_AV_PARAM * pAvParam,const char * filePath,int nFileMaxSeconds,int nAllRecordMaxSeconds);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_InputRecordStream(LONG lRealHandle,const void * pBuffer,int nSize,int isVideo,int iskey,double timestamp);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_StopRecordStream(LONG lRealHandle);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_wzwTest();

	/*
	* 抓图保存在用户提供的文件名中
	* lUserID 登陆id
	* bsub, 是否子码流, 0,主码流, 1子码流
	* quality, 抓图质量, 0~100, 越高质量越好
	* filename 抓图要保存的文件全名称,如"test.jpeg", 不带路径则在当前目录下
	* timeout, 超时时间, 单位ms, 填0则非阻塞，需要在OnStateEvent回调中处理抓图报警。填其他值则等待相应的时间直到抓图成功。
	* 返回值, 0 ok, 其他错误码
	*/
	NETSDKDLL_API       LONG         __stdcall	    IP_NET_DVR_SnapPic(LONG lUserID, int bsub, int quality, const char* filename, int timeout);
	
	//[内部使用]直接将xml发送到IPC，用于透明通道中转(可附带文件数据于xml之后)
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SendRawMsg(LONG lUserID, const char *xmlBuf, const int len);


	typedef struct SnapPicTask_S
	{
		LONG userid[64]; // user id 列表
		int usercount;  // 实际用户个数
		int bsub;
		int quality;
		char* buf[64];   // 每个用户的抓图内存指针 , 
		int buflen[64]; // 内存实际长度, 最好要200k 左右

		int snapInterval;// 每个ipc 直接抓图间隔
	}SnapPicTask;

	/* 提交抓图任务后 回调函数在事件 EVENT_DOWNLOADOK 返回抓图结果, 数据指针转换成以下结构 */
	typedef struct SnapPicResult_S
	{
		char* picbuf;
		int piclen;
	}SnapPicResult;

	LONG __stdcall	IP_NET_DVR_SnapPicTaskStart(SnapPicTask* task);
	LONG __stdcall	IP_NET_DVR_SnapPicTaskStop();

	/*
	去XML，使用结构体来获取/设置
	设置前，请确保数据正确，否则可能导致设备无法启动(最好先获取一遍，然后修改需要修改的项目)
	*/

	/*
	从XML解析出结构体
	*/
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetNetworkCfgByXml(NetworkConfigNew *pNetworkCfg, char *xmlBuf);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Network_getLANCfgByXml(LANConfig *lanCfg,char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Network_getWIFICfgByXml(WIFIConfig *wifiCfg,char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Network_getADSLCfgByXml(ADSLConfigNew *adslCfg,char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Network_getDDNSCfgByXml(DDNSConfig *ddnsCfg,char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Network_getUPNPCfgByXml(UPNPConfig *upnpCfg,char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Network_getP2PCfgByXml(P2PConfig *p2pCfg,char *xmlBuf);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Server_getFtpsByXml(ServerConfig *pServerCfg, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Server_getSmtpsByXml(ServerConfig *pServerCfg, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetServerCfgByXml(ServerConfig *pServerCfg, char *xmlBuf);


	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetRecordCfgByXml(RecordConfig *pRecordCfg, char *xmlBuf);


	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Media_getAudioByXml(AudioConfig *pAudioCfg,  char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Media_getVideoByXml(VideoConfig *pVideoCfg, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetMediaCfgByXml(MediaConfig *pMediaCfg, char *xmlBuf);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Media_getVideoCaptureByXml(VideoCapture *pVideoCapture, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Media_getVideoOverlayByXml(VideoOverlay *pVideoOverlay, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Media_getVideoEncodeByXml(VideoEncode *pVideoEncode, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Media_getJpegEncodeByXml(JpegEncodeCfg *pJpegCfg, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Media_getVideoMaskByXml(VideoMaskConfig *pVideoMask, char *xmlBuf);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetMediaStreamCfgByXml(MediaStreamConfig *pMediaStream, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetPlatformCfgByXml(PlatformConfig *pPlatform, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetGB28181CfgByXml(GB28181Config *pPlatformCfg, char *xmlBuf);


	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Alarm_getInputByXml(InputAlarm *pInputAlm, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Alarm_getMotionDetectByXml(MotionDetectAlarm *pMDAlm, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Alarm_getPersonDetectByXml(PdAlarm *pPDAlm, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Alarm_getVideoLostByXml(VideoLostAlarm *pVideoLost, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Alarm_getVideoCoverByXml(VideoCoverAlarm *pVideoCover, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_Alarm_getStorageFullByXml(StorageFullAlarm *pSFAlm, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetAlarmConfigByXml(AlarmConfig *pAlarmCfg, char *xmlBuf);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_InputAlarm_getAlarmChannelCfgByXml(AlarmChannel *pAlarmChannel, char *xmlBuf);


	NETSDKDLL_API LONG __stdcall IP_NET_DVR_System_getPTZCfgByXml(PTZConfig *pPtzCfg, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_System_getTimeCfgByXml(TimeConfig *pTimeCfg, char *xmlBuf);	
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_System_getUserCfgByXml(UserConfig *pUserCfg, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_System_getLogCfgByXml(SyslogConfig *pSyslogCfg, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_System_getMiscCfgByXml(MiscConfig *pMiscCfg, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetSystemConfigByXml(SystemConfig *pSystemCfg, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetSystemVersionByXml(SYSTEM_VERSION_DATA *pSystemVer, char *xmlBuf);


	NETSDKDLL_API LONG __stdcall IP_NET_DVR_System_getPTZCommonCfgByXml(PTZCommonConfig *pPtzCommonCfg, char *xmlBuf);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_System_getPTZAdvanceCfgByXml(PTZAdvanceConfig *pPtzAdvanceCfg, char *xmlBuf);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GetReplayDistributeByXml(char *distribute, char *xmlBuf);

	/*
	将结构体转换成XML
	XML内存为动态malloc ，用完后需要调用者手动free，否则会造成内存泄漏
	*/
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_SystemConfig(SystemConfig *pSystemCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_UserConfig(UserConfig *pUserCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_TimeConfig(TimeConfig *pTimeCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_SyslogConfig(SyslogConfig *pSyslogCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_SysMiscConfig(MiscConfig *pMiscCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_PTZConfig(PTZConfig *pPtzCfg);

	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_AlarmConfig(AlarmConfig *pAlarmCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_InputAlarmConfig(InputAlarm *pInputAlm);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_MDAlarmConfig(MotionDetectAlarm *pMDAlm);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_PDAlarmConfig(PdAlarm *pPDAlm);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_VlAlarmConfig(VideoLostAlarm *pVideoLost);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_VCAlarmConfig(VideoCoverAlarm *pVideoCover);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_SFAlarmConfig(StorageFullAlarm *pSFAlm);


	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_MediaStreamConfig(MediaStreamConfig *mediaStreamCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_PlatformConfig(PlatformConfig *platformCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_GB28181Config(GB28181Config *platformCfg);


	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_RecordConfig(RecordConfig *recordCfg);


	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_NetworkConfig(NetworkConfigNew *networkCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_NetworkLANConfig(LANConfig *lanCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_NetworkWIFIConfig(WIFIConfig *wifiCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_NetworkADSLConfig(ADSLConfigNew *adslCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_NetworkDDNSConfig(DDNSConfig *ddnsCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_NetworkUPNPConfig(UPNPConfig *upnpCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_NetworkP2PConfig(P2PConfig *pCfg);

	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_ServerConfig(ServerConfig *serverCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_FtpConfig(FtpServerList *fptServerList);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_SmtpConfig(SmtpServerList *smtpServerList);


	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_MediaConfig(MediaConfig *mediaCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_AudioConfig(AudioConfig *audioCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_VideoConfig(VideoConfig *videoCfg);

	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_AudioCaptureConfig(AudioCapture* audioCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_AudioEncodeConfig(AudioEncode *audioCfg);

	

	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_VideoOverlayConfig(VideoOverlay *pCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_VideoMaskConfig(VideoMaskConfig *pCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_VideoCaptureConfig(VideoCapture*pCfg);
	NETSDKDLL_API char* __stdcall IP_NET_DVR_XMLGET_VideoEncodeConfig(VideoEncode*pCfg);

	/*detail system config get*/
	/*GET函数调用时仅仅发网络请求，返回结果在回调中进行处理*/
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_AlarmConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_MotionDetectAlarm(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_PersonDetectAlarm(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_InputAlarm(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_VideoLostAlarm(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_VideoCoverAlarm(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_StorageFullAlarm(LONG lUserID);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_SystemConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_PtzConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_UserConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_SyslogConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_TimeConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_MiscConfig(LONG lUserID);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_RecordConfig(LONG lUserID);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_MediaConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_VideoConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_AudioConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_VideoOSDConfig(LONG lUserID);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_MediaStreamConfig(LONG lUserID);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_PlatformConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_GB28181Config(LONG lUserID);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_NetworkConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_NetworkLANConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_NetworkWIFIConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_NetworkADSLConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_NetworkDDNSConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_NetworkUPNPConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_NetworkP2PConfig(LONG lUserID);

	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_ServerConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_FtpServerConfig(LONG lUserID);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_SmtpServerConfig(LONG lUserID);

	/*detail system config set*/
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_ModuleConfig(LONG lUserID, int moduleid,char *xmlconfig);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_MotionDetectAlarm(LONG lUserID, MotionDetectAlarm *pMDAlm);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_PersonDetectAlarm(LONG lUserID, PdAlarm *pPDAlm);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_VideoCoverAlarm(LONG lUserID, VideoCoverAlarm *pVideoCover); 
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_MiscConfig(LONG lUserID, MiscConfig *pCfg);  
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_MediaConfig(LONG lUserID, MediaConfig *pConfig);   
	
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_VideoCaptureConfig(LONG lUserID, VideoCapture *pCaptureCfg); 
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_VideoEncodeConfig(LONG lUserID, VideoEncode *pEncodeCfg);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_VideoOSDConfig(LONG lUserID, VideoOverlay *pCfg);	
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_VideoUserOSDConfig(LONG lUserID, VideoUserOverlay *p_config);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_VideoMaskConfig(LONG lUserID, VideoMaskConfig *pCfg);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_VideoConfig(LONG lUserID, VideoConfig *pVideoCfg);   
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_AudioConfig(LONG lUserID, AudioConfig *pAudioCfg);   
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_MediaStreamConfig(LONG lUserID, MediaStreamConfig *config);  
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_PlatformConfig(LONG lUserID, PlatformConfig *pPlatformCfg);  
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_GB28181Config(LONG lUserID, GB28181Config *pPlatformCfg);
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_UserConfig(LONG lUserID, UserConfig *pUserCfg);  
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_TimeConfig(LONG lUserID, TimeConfig *pTimeCfg);  
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_NetworkLANConfig(LONG lUserID, LANConfig *config);   
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_NetworkWIFIConfig(LONG lUserID, WIFIConfig *config); 
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_NetworkConfig(LONG lUserID, NetworkConfigNew *config);   

	//编码能力集XML解析API.需要调用者delete pOutVideoCap pOutAudioCap
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_MediaCapability( LONG lUserID  ) ;
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_XMLGET_MediaCapability(char *pRespone, 
		int *pVideoCapCount, RESOLUTION_ENTRY **pOutVideoCap, 
		int *pAudioCapCount, AUDIO_CODEC_ENTRY **pOutAudioCap )  ;

///////////IPCAMERA时间相关API//////////////////
//	时区转换:-12~12时区转换成IPCAMERA的时区
#define TRANS_TIMEZONE_TO_IPCAMERA(tz)  ((tz + 12)*60)		
	typedef struct
	{
		int timezone ; //0~1500
		int year; 
		int month; 
		int day;
		int hour; 
		int min; 
		int second; 
	}AjTime;

	//获取IPCAMERA时间
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_GET_SYSTEMTIME( LONG lUserID  ) ;
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_XMLGET_SYSTEMTIME( char *pRespone, AjTime *pTime  ) ;	
	//设置IPCAMERA时间和时区
	NETSDKDLL_API LONG __stdcall IP_NET_DVR_SET_SYSTEMTIME( LONG lUserID, AjTime *pTime ) ;


	
	NETSDKDLL_API const char* __stdcall IP_NET_DVR_GET_EVENTNAME( LONG nEvent) ;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


