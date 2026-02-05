#ifndef	__AJNETSDKDLL_20160716_H
#define __AJNETSDKDLL_20160716_H

#include "media_cfg.h"
#include "data_struct.h"
#include "cmd_def.h"

#ifdef AJNETSDKDLL_EXPORTS
#define AJNETSDKDLL_API __declspec(dllexport)
#else
#ifdef AJNETSDKDLL_IS_LIB
#define AJNETSDKDLL_API
#else
#define AJNETSDKDLL_API __declspec(dllimport)
#pragma comment(lib, "AjNetSdkDll.lib")
#endif
#endif

enum Aj_ConnectType {
	Aj_Type_Udp = 0,
	Aj_Type_Tcp,
	Aj_Type_Multicast
};

typedef enum _AudioType_e {
	AudioType_UNINIT = -1,
	AudioType_PCMU,
	AudioType_AAC_LC,
	AudioType_PCMA,
	AudioType_PCM,
	AudioType_OPUS,
	AudioType_MP3,
}AudioType_e;

enum REPLAY_IPC_ACTION
{
	ACTION_PLAY = 0,
	ACTION_PAUSE,
	ACTION_RESUME,
	ACTION_FAST,
	ACTION_SLOW,
	ACTION_SEEK,
	ACTION_FRAMESKIP,
	ACTION_STOP,
	ACTION_CAPIMG = 10,
	ACTION_CHANGE_SOUND,
	ACTION_RECV_DECODEPARAM,
};

///////////////////////////////////////////////////////////////////////////
//Struct
//////////////////////////////////////////////////////////////////////////

#define MAX_IPC_SERIALNUMBER	32
#define MAX_DEVICETYPE_LEN		32
#define MAX_IPC_P2PID_LEN		128

typedef struct
{
	char				ipc_sn[MAX_IPC_SERIALNUMBER];
	char				platform_id[MAX_IPC_SERIALNUMBER];
	char				p2p_id[MAX_IPC_P2PID_LEN];
	char				deviceType[MAX_DEVICETYPE_LEN];
	char				version[MAX_DEVICETYPE_LEN];
	char				title[256];
	UserConfig			userCfg;
	MediaStreamConfig	streamCfg;
	LANConfig			lanCfg;
	int					p2pStatus;
	int					runnedtime;
	int					bChanged;
	int					ChannelNum;
	int					factoryMode;
}IPC_ENTRY_AJ;

//typedef struct _FRAMNE_INFO
//{
//	int bIsVideo;
//	int bIsKeyFrame;
//	double TimeStamp;
//}FRAMNE_INFO;

#define ALARM_ITEM ALARM_ENTRY

typedef struct
{
	LONG    lChannel;
	LONG    lLinkMode;
	HWND    hPlayWnd;
	char    *sMultiCastIP;
}*LPAJ_NETSDK_IPC_CLIENTINFO, AJ_NETSDK_IPC_CLIENTINFO;

//#define			SERIALNO_LEN		48
//typedef struct
//{
//	BYTE     sSerialNumber[SERIALNO_LEN];
//	BYTE     byAlarmInPortNum;
//	BYTE     byAlarmOutPortNum;
//	BYTE     byDiskNum;
//	BYTE     byDVRType;
//	BYTE     byChanNum;
//	BYTE     byStartChan;
//}*LPAJ_NETSDK_IPC_DEVICEINFO, AJ_NETSDK_IPC_DEVICEINFO;
//
typedef struct
{
}AJ_NETSDK_IPC_ALARMER;

#define MAX_IPADDR_LEN	64

enum ERROR_CODE
{
	ERR_NOT_FIND_DEVICE = -9000002,
	ERR_OPEN_AUDIOCAPTURE_FAIL,
	ERR_START_AUDIOCAPTURE_FAIL,
	ERR_AUDIO_PARAM_ERROR,
	ERR_AUDIO_NOT_START,
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
	ERR_DLL_NOINITORRELEASE_ERROR,
	ERR_SERIAL_NOT_START,
	ERR_TCP_CONNECT_ERROR,
};

enum enumNetSatateEvent
{
	EVENT_CONNECTING,
	EVENT_CONNECTOK,
	EVENT_CONNECTFAILED,
	EVENT_SOCKETERROR,
	EVENT_LOGINOK,
	EVENT_LOGINFAILED,
	EVENT_STARTAUDIOOK,
	EVENT_STARTAUDIOFAILED,
	EVENT_STOPAUDIOOK,
	EVENT_STOPAUDIOFAILED,
	EVENT_SENDPTZOK,
	EVENT_SENDPTZFAILED,
	EVENT_SENDAUXOK,
	EVENT_SENDAUXFAILED,
	EVENT_UPLOADOK,
	EVENT_UPLOADFAILED,
	EVENT_DOWNLOADOK,
	EVENT_DOWNLOADFAILED,
	EVENT_REMOVEOK,
	EVENT_REMOVEFAILED,
	EVENT_SENDPTZERROR,
	EVENT_PTZPRESETINFO,
	EVNET_PTZNOPRESETINFO,
	EVENT_PTZALARM,
	EVENT_RECVVIDEOPARAM,
	EVENT_RECVAUDIOPARAM,
	EVENT_CONNECTRTSPERROR,
	EVENT_CONNECTRTSPOK,
	EVENT_RTSPTHREADEXIT,
	EVENT_URLERROR,
	EVENT_RECVVIDEOAUDIOPARAM,
	EVENT_LOGIN_USERERROR,
	EVENT_LOGOUT_FINISH,
	EVENT_LOGIN_RECONNECT,
	EVENT_LOGIN_HEARTBEAT_LOST,
	EVENT_STARTAUDIO_ISBUSY,
	EVENT_STARTAUDIO_PARAMERROR,
	EVENT_STARTAUDIO_AUDIODDISABLED,
	EVENT_CONNECT_RTSPSERVER_ERROR,
	EVENT_CREATE_RTSPCLIENT_ERROR,
	EVENT_GET_RTSP_CMDOPTION_ERROR,
	EVENT_RTSP_AUTHERROR,
	EVNET_RECORD_FILEBEGIN,
	EVENT_RECORD_FILEEND,
	EVENT_RECORD_TASKEND,
	EVENT_RECORD_DISKFREESPACE_TOOLOW,
	EVNET_RECORD_FILEBEGIN_ERROR,
	EVNET_RECORD_WRITE_FILE_ERROR,
	EVENT_RECORD_INITAVIHEAD_ERROR,
	EVENT_RECORD_MEDIA_PARAM_ERROR,
	EVENT_NVR_CHANNELS,
	EVENT_NVR_IPC_STATUS,
	EVENT_SYSTEMREBOOT_ANDRELOGINOK,
	EVENT_NETWORKRESET_ANDRELOGINOK,
	EVENT_UPLOAD_PROCESS,
	EVENT_DOWNLOAD_PROCESS,
	EVENT_DOWNLOAD_RETRY_ANDRESTAR,
	EVENT_LOGOUT_BYUSER,
	EVENT_P2P_CONNECT_STATE_INFO,
	EVNET_INITP2P_OK,
	EVNET_INITP2P_ERROR,
	EVENT_START_CONNECT_DEVICE,
	EVENT_START_CONNECT_DEVICE_ERROR,
	EVENT_P2PSERVER_LOGIN_OK,
	EVENT_P2PSERVER_LOGOUT,
	EVENT_P2PERROR_EVNETINFO,
	EVENT_P2PCONNECT_DEVICEOK,
	EVENT_P2PCONNECT_CLOSE,
	EVENT_P2P_EXIT_CONNECT,
	EVENT_CAPTURE_IMAGE_FINISH,
	EVENT_RECVABLITY_INFO,
	EVENT_P2P_CLINET_CHANNLEINFO,
	EVENT_P2P_STARTSTREAM_ERROR11,
};

enum SEARCH_EVENT
{
	EVENT_SEARCH_RECV_NEWIPCINFO = 1,
	EVENT_SEARCH_UPDATEINFO,
};

enum PTZ_CMD_TYPE
{
	LIGHT_PWRON = 2,
	WIPER_PWRON,
	FAN_PWRON,
	HEATER_PWRON,
	AUX_PWRON1,
	AUX_PWRON2,
	ZOOM_IN_VALUE = 11,
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
#define ZOOM_IN		ZOOM_IN_VALUE
#define ZOOM_OUT	ZOOM_OUT_VALUE
#endif

enum PTZ_PRESET_TYPE
{
	SET_PRESET = 8,
	CLE_PRESET = 9,
	GOTO_PRESET = 39
};

#ifndef X_MC

#ifndef __UNICLIENT__201111171107__H__
#define __UNICLIENT__201111171107__H__

typedef struct
{
	int		bIsKey;
	double	timestamp;
	void    *pUserData;
}FRAME_EXTDATA, *LPFRAME_EXTDATA;

//playback data head info
typedef struct _updpackhead
{
	unsigned long frame_timestamp;
	unsigned long keyframe_timestamp;
	unsigned short pack_seq;
	unsigned short payload_size;
	unsigned char pack_type;//0x01 first packet锟斤拷x10 last packet, 0x11 first and last packet锟斤拷x00 middle packet
	unsigned char frame_type;
	unsigned char stream_type;//0: video, 1: audio
	unsigned char stream_index;
	unsigned int  frame_index;
}UpdPackHead;

enum FILE_TYPE
{
	LOG_FILE,
	RECORD_FILE,
	CONFIG_FILE,
	UPDATE_FILE
};
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
	typedef LONG(CALLBACK *SearchIPCCallBack)(LONG nEventCode, LONG index, IPC_ENTRY_AJ *pResponse, void *pUser);
	typedef LONG(CALLBACK *FactoryCallBack)(long nEventCode, char *szDevSn, long index, long LedMode, long IrcutMode, void *pUser);
	typedef LONG(CALLBACK *StatusEventCallBack)(LONG lUser, LONG nStateCode, char *pResponse, void *pUser);
	typedef LONG(CALLBACK *AUXResponseCallBack)(LONG lUser, LONG nType, char *pResponse, void *pUser);
	typedef LONG(CALLBACK *fVoiceDataCallBack)(LONG lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, LPFRAME_EXTDATA  pUser);
	typedef LONG(CALLBACK *fRealDataCallBack)(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LPFRAME_EXTDATA  pExtData);
	typedef LONG(CALLBACK *fPlayActionEventCallBack)(LONG lUser, LONG nType, LONG nFlag, char * pData, void * pUser);
	typedef	LONG(CALLBACK* fEncodeAudioCallBack)(long lType, long lPara1, long lPara2);
	typedef LONG(CALLBACK* SerialDataCallBack) (LONG lUser, char *pRecvDataBuffer, DWORD dwBufSize, void *pUser);
	typedef	LONG(CALLBACK* fCaptureAudioCallBack)(BYTE* pData, DWORD dwBufSize, void* pUser);
	typedef LONG(CALLBACK* fRecFileNameCallBack) (LONG lRealHandle, char *pRecFileNameBuf, DWORD dwBufSize, void *pUser);

	//////////////////////////////////////////////////////////////////////////
	//系统初始化及相关信息
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Init();
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_LoadVendorId();
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Cleanup();
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetSDKBuildData();
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetSDKVersion();
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetTimeSync(LONG lEnable, LONG lMax_diff, LONG lInterval);
	//全局callback
	//搜索设备结果回调函数
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetSearchStatusCallBack(SearchIPCCallBack fcallBack, void * pUser);
	//设备状态回调函数
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetStatusEventCallBack(StatusEventCallBack fStatusEventCallBack, void * pUser);
	//辅助通道消息回调函数
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetAUXResponseCallBack(AUXResponseCallBack fAUXCallBack, void * pUser);
	//前端录像回放媒体数据回调函数
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetReplayDataCallBack(fRealDataCallBack cbReplayDataCallBack, void * dwUser);
	//前端录像回放事件消息回调函数
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetPlayActionEventCallBack(fPlayActionEventCallBack cbActionCallback, void * dwUser);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetFactoryCallBack(FactoryCallBack cbFactoryCallBack, void * dwUser);
	//设备搜索功能
	AJNETSDKDLL_API	LONG  AJ_NETSDK_IPC_StartSearchIPC();
	AJNETSDKDLL_API	LONG  AJ_NETSDK_IPC_StopSearchIPC();
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SendSearchMsg();
	AJNETSDKDLL_API	LONG  AJ_NETSDK_IPC_GetSearchIPCCount();
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetIPCInfo(LONG index, IPC_ENTRY_AJ * pIPCInfo);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_ModifyIPC(LONG index, IPC_ENTRY_AJ * pIPCInfo);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetOneIPAddress(char * strResult, int nSize);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetNetworkParam(ULONG nParamIndex, char * strResult, int nSize);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_RestoreIPC(LONG index, IPC_ENTRY_AJ * pIPCInfo);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_RestoreIPCBySn(char*sDevSn);
	AJNETSDKDLL_API long  AJ_NETSDK_IPC_OpenFactory(char*sDevSn);
	AJNETSDKDLL_API long  AJ_NETSDK_IPC_SetFactoryCfg(char*sDevSn, int code, int LedMode, int IrcutMode);
	//设备登陆与连接
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetAutoReconnect(LONG lUserID, int bAutoReconnect);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Reconnect(LONG lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Login(char *sDVRIP, WORD wDVRPort, char *sUserName, char *sPassword, void* pBlock);
	AJNETSDKDLL_API LONG  AJ_NETSDK_Server_Login(char * sDVRIP, WORD wDVRPort, char * sUserName, char * sPassword, char *sUserIdentity, void* pBlock);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Logout(long lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_LogoutAll();
	/************************************************************************
	功能：配置对讲参数
	参数：
	connectType：连接模式 参考Aj_ConnectType结构。当组播模式下 只需要调用AJ_NETSDK_IPC_StartVoiceCom接口，不需要调用AJ_NETSDK_IPC_AddTalk接口。
	destAudiotype：设备端接受的音频格式，参考AudioType_e.
	samplerate:设备端接受的采样率。
	channels：设备端接受的通道数。
	返回值：0表示成功。
	< 0 表示失败，参考错误码。
	************************************************************************/
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_StartTalk(int connectType, int destAudiotype, int samplerate, int bitspersample, int channels);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_StopTalk();
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_StartAudioCapture(int samplerate, int bitspersample, int channels, fCaptureAudioCallBack cbCaptureAudioCallBack, void* pUser);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_StopAudioCapture();
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_AddTalk(LONG lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_RemoveTalk(LONG lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_AddInviteAudioStream(LONG * lPort);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_RemoveInviteAudioStream(LONG lPort);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetEncodeAudioCallBack(LONG nPort, fEncodeAudioCallBack fnCallBack);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_StartVoiceCom(LONG lUserID, DWORD dwVoiceChan, BOOL bNeedCBNoEncData, fVoiceDataCallBack cbVoiceDataCallBack, void *pUser);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_StopVoiceCom(LONG lVoiceComHandle);
	//自定义对讲数据接口
	//srcAudioType 输入的音频格式。
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_InputAudioData(int srcAudioType, BYTE* pBuff, int len);
	//设备配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetDeviceAbility(LONG lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetDeviceConfig(LONG lUserID, DWORD dwCommand);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetDevcieConfig(LONG lUserID, DWORD dwCommand, LPVOID pInBuffer, DWORD dwInBufferSize);
	//保留接口（一起用新的接口代替）
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetDVRConfig(LONG lUserID, DWORD dwCommand, LONG lChannel, LPVOID lpOutBuffer, DWORD dwOutBufferSize, LPDWORD lpBytesReturned);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SystemControl(LONG lUserID, DWORD nCmdValue, LONG flag, char * pXml);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetDVRConfig(LONG lUserID, DWORD dwCommand, LONG lChannel, LPVOID pXml, DWORD dwInBufferSize);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_CreateIFrame(ULONG lUserId, int bIsSubStream);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_RebootDVR(LONG lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_RestoreConfig(LONG lUserID);
	//获取设备配置新接口  细分获取配置，、
	//请求成功后获取的配置将在辅助通道回调函数中得到
	//辅助通道回调通过AJ_NETSDK_IPC_SetAUXResponseCallBack()设置
	//辅助通道回调中获得的配置信息是一串xml字符串 可以通过AJ_NETSDK_IPC_GetxxxByXml()接口解析转换得到相应的结构体
	//流媒体配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_MediaStreamConfig(LONG lUserID);
	//媒体设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_MediaConfig(LONG lUserID);
	//视频配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_VideoConfig(LONG lUserID);
	//视频采集设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_VideoCaptureConfig(LONG lUserID);
	//视频编码设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_VideoEncodeConfig(LONG lUserID);
	//音频设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_AudioConfig(LONG lUserID);
	//音频采集配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_AudioCaputreConfig(LONG lUserID);
	//音频编码设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_AudioEncodeConfig(LONG lUserID);
	//告警配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_AlarmConfig(LONG lUserID);
	//移动侦测
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_MotionDetectAlarm(LONG lUserID);
	//IO输入告警参数
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_InputAlarm(LONG lUserID);
	//视频丢失告警参数
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_VideoLostAlarm(LONG lUserID);
	//视频遮挡配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_VideoCoverAlarm(LONG lUserID);
	//存储满告警配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_StorageFullAlarm(LONG lUserID);
	//设备系统配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_SystemConfig(LONG lUserID);
	//云台配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_PtzConfig(LONG lUserID);
	//用户配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_UserConfig(LONG lUserID);
	//系统日志配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_SyslogConfig(LONG lUserID);
	//时区设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_TimeConfig(LONG lUserID);
	//设备系统语言设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_MiscConfig(LONG lUserID);
	//前端录像设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_RecordConfig(LONG lUserID);
	//osd设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_VideoOSDConfig(LONG lUserID);
	//平台设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_PlatformConfig(LONG lUserID);
	//28181配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_GB28181Config(LONG lUserID);
	//网络配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_NetworkConfig(LONG lUserID);
	//局域网配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_NetworkLANConfig(LONG lUserID);
	//wifi配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_NetworkWIFIConfig(LONG lUserID);
	//ADSL拔号配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_NetworkADSLConfig(LONG lUserID);
	//DDNS配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_NetworkDDNSConfig(LONG lUserID);
	//UPNP配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_NetworkUPNPConfig(LONG lUserID);
	//P2P设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_NetworkP2PConfig(LONG lUserID);
	//FTP&SMTP相关配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_ServerConfig(LONG lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_FtpServerConfig(LONG lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GET_SmtpServerConfig(LONG lUserID);
	//设置设备配置新接口  采用结构体不在使用Xml配置
	//流媒体设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_MediaStreamConfig(LONG lUserID, MediaStreamConfig *pConfig);
	//媒体设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_MediaConfig(LONG lUserID, MediaConfig *pConfig);
	//视频配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_VideoConfig(LONG lUserID, VideoConfig *pConfig);
	//视频采集设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_VideoCaptureConfig(LONG lUserID, VideoCapture *pConfig);
	//视频编码设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_VideoEncodeConfig(LONG lUserID, VideoEncode *pConfig);
	//OSD时间标题设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_VideoOSDConfig(LONG lUserID, VideoOverlay *pConfig);
	//音频设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_AudioConfig(LONG lUserID, AudioConfig *pConfig);
	//音频采集设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_AudioCaputreConfig(LONG lUserID, AudioCapture *pConfig);
	//音频编码设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_AudioEncodeConfig(LONG lUserID, AudioEncode *pConfig);
	//告警配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_AlarmConfig(LONG lUserID, AlarmConfig* pConfig);
	//移动侦测
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_MotionDetectAlarm(LONG lUserID, MotionDetectAlarm *pConfig);
	//视频遮挡
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_VideoCoverAlarm(LONG lUserID, VideoCoverAlarm *pConfig);
	//隐私遮挡设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_VideoMaskConfig(LONG lUserID, VideoMaskConfig *pConfig);
	//设备默认语言设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_MiscConfig(LONG lUserID, MiscConfig *pConfig);
	//平台设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_PlatformConfig(LONG lUserID, PlatformConfig *pConfig);
	//28181设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_GB28181Config(LONG lUserID, GB28181Config *pConfig);
	//用户列表设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_UserConfig(LONG lUserID, UserConfig *pConfig);
	//时间同步设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_TimeConfig(LONG lUserID, TimeConfig *pConfig);
	//当时间同步为手动时设置设备时间
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_TimeManualConfig(LONG lUserID, TimeManualConfg *pConfig);
	//局域网设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_NetworkLANConfig(LONG lUserID, LANConfig *pConfig);
	//Wifi设置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_NetworkWIFIConfig(LONG lUserID, WIFIConfig *pConfig);
	//网络配置
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SET_NetworkConfig(LONG lUserID, NetworkConfigNew *pConfig);

	//系统上传和下载文件（包括更新固件）
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Upgrade(LONG lUserID, char *sFileName);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetConfigFile(LONG lUserID, char *sFileName);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetUpgradeProgress(LONG lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetUpgradeState(LONG lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_CloseUpgradeHandle(LONG lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetFileByName(LONG lUserID, LONG nFileType, char *sDVRFileName, char *saveDir);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetConfigFile(LONG lUserID, char *sFileName);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_StopGetFile(LONG lFileHandle);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetDownloadState(LONG lFileHandle);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetDownloadPos(LONG lFileHandle);
	//云台控制
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_PTZControl(LONG lUser, DWORD dwPTZCommand, DWORD nTspeed, DWORD nSpeed);
	//此接口支持485透传，支持自定义云台消息
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_PTZControlEx(LONG lUser, char *pXml);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_PTZPreset(LONG lUser, DWORD dwPTZPresetCmd, DWORD dwPresetIndex);
	//格式化存储设备
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_FormatDisk(LONG lUserID, LONG lDiskNumber);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetFormatProgress(LONG lFormatHandle, LONG *pCurrentFormatDisk, LONG *pCurrentDiskPos, LONG *pFormatStatic);
	//设置和获取用户保存数据
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetUserData(ULONG lUserId, char * pOutBuffer, int* nInOutLen);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SetUserData(ULONG lUserId, char * pBuffer, int len);

	//ipc文件回放
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_ControlPlay(LONG lUserID, int iReplayMode, LONG Action, LONG param);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_PlayDeviceByFile(LONG lUserID, char * filenme);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_PlayDeviceByTime(LONG lUserID, long lTimescamp);

	//串口
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SerialStart(LONG lUserId, SerialDataCallBack cbSDCallBack, void* pUser);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SerialSend(LONG lUserId, LONG lChannel, char *pSendBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SerialStop(LONG lUserId);
	//add by Alvin.Chen @20170310
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetIOInPutStatus(LONG lUserID, int nChannelNo);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetIOOutPutStatus(LONG lUserID, int nChannelNo);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_UploadOEMAppFile(LONG lUserID, char* filePath);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_UploadOEMCfgFile(LONG lUserID, char* filePath);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_UploadOEMMp3File(LONG lUserID, char* filePath);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_SearchOEMMp3File(LONG lUserID, int iPage);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_PlayMp3File(LONG lUserID, char* filePath, int nTimes);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_StopPlayMp3File(LONG lUserID);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetPlayAudioStatus(LONG lUserID);
	/*
	去XML，使用结构体来获取/设置
	设置前，请确保数据正确，否则可能导致设备无法启动(最好先获取一遍，然后修改需要修改的项目)
	*/

	/*
	从XML解析出结构体
	*/
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetNetworkCfgByXml(NetworkConfigNew *pNetworkCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Network_getLANCfgByXml(LANConfig *lanCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Network_getWIFICfgByXml(WIFIConfig *wifiCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Network_getADSLCfgByXml(ADSLConfigNew *adslCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Network_getDDNSCfgByXml(DDNSConfig *ddnsCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Network_getUPNPCfgByXml(UPNPConfig *upnpCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Network_getP2PCfgByXml(P2PConfig *p2pCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Server_getFtpsByXml(ServerConfig *pServerCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Server_getSmtpsByXml(ServerConfig *pServerCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetServerCfgByXml(ServerConfig *pServerCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetRecordCfgByXml(RecordConfig *pRecordCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Media_getAudioByXml(AudioConfig *pAudioCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Media_getVideoByXml(VideoConfig *pVideoCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetMediaCfgByXml(MediaConfig *pMediaCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Media_getVideoCaptureByXml(VideoCapture *pVideoCapture, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Media_getVideoOverlayByXml(VideoOverlay *pVideoOverlay, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Media_getVideoEncodeByXml(VideoEncode *pVideoEncode, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Media_getJpegEncodeByXml(JpegEncodeCfg *pJpegCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Media_getVideoMaskByXml(VideoMaskConfig *pVideoMask, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetMediaStreamCfgByXml(MediaStreamConfig *pMediaStream, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetPlatformCfgByXml(PlatformConfig *pPlatform, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetGB28181CfgByXml(GB28181Config *pPlatformCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Alarm_getInputByXml(InputAlarm *pInputAlm, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Alarm_getMotionDetectByXml(MotionDetectAlarm *pMDAlm, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Alarm_getVideoLostByXml(VideoLostAlarm *pVideoLost, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Alarm_getVideoCoverByXml(VideoCoverAlarm *pVideoCover, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Alarm_getStorageFullByXml(StorageFullAlarm *pSFAlm, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetAlarmConfigByXml(AlarmConfig *pAlarmCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_InputAlarm_getAlarmChannelCfgByXml(AlarmChannel *pAlarmChannel, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_System_getPTZCfgByXml(PTZConfig *pPtzCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_System_getTimeCfgByXml(TimeConfig *pTimeCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_System_getUserCfgByXml(UserConfig *pUserCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_System_getLogCfgByXml(SyslogConfig *pSyslogCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_System_getMiscCfgByXml(MiscConfig *pMiscCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_GetSystemConfigByXml(SystemConfig *pSystemCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_System_getPTZCommonCfgByXml(PTZCommonConfig *pPtzCommonCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_System_getPTZAdvanceCfgByXml(PTZAdvanceConfig *pPtzAdvanceCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Media_getAudioCaptureByXml(AudioCapture *pCaptureCfg, char *xmlBuf);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Media_getAudioEncodeByXml(AudioEncode *pEncodeCfg, char *xmlBuf);

	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_Media_getVideoUserOverlayByXml(VideoUserOverlay *pVideoOverlay, char *xmlBuf);
	/*
	将结构体转换成XML
	XML内存为动态malloc ，用完后需要调用者手动free，否则会造成内存泄漏
	释放内存必须使用AJ_NETSDK_IPC_XMLGET_SAFE_FREE 接口进行释放。
	*/
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_SystemConfig(SystemConfig *pSystemCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_UserConfig(UserConfig *pUserCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_TimeConfig(TimeConfig *pTimeCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_SyslogConfig(SyslogConfig *pSyslogCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_SysMiscConfig(MiscConfig *pMiscCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_PTZConfig(PTZConfig *pPtzCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_AlarmConfig(AlarmConfig *pAlarmCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_InputAlarmConfig(InputAlarm *pInputAlm);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_MDAlarmConfig(MotionDetectAlarm *pMDAlm);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_VlAlarmConfig(VideoLostAlarm *pVideoLost);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_VCAlarmConfig(VideoCoverAlarm *pVideoCover);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_SFAlarmConfig(StorageFullAlarm *pSFAlm);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_MediaStreamConfig(MediaStreamConfig *mediaStreamCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_PlatformConfig(PlatformConfig *platformCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_GB28181Config(GB28181Config *platformCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_RecordConfig(RecordConfig *recordCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_NetworkConfig(NetworkConfigNew *networkCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_NetworkLANConfig(LANConfig *lanCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_NetworkWIFIConfig(WIFIConfig *wifiCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_NetworkADSLConfig(ADSLConfigNew *adslCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_NetworkDDNSConfig(DDNSConfig *ddnsCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_NetworkUPNPConfig(UPNPConfig *upnpCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_NetworkP2PConfig(P2PConfig *pCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_ServerConfig(ServerConfig *serverCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_FtpConfig(FtpServerList *fptServerList);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_SmtpConfig(SmtpServerList *smtpServerList);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_MediaConfig(MediaConfig *mediaCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_AudioConfig(AudioConfig *audioCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_VideoConfig(VideoConfig *videoCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_VideoOverlayConfig(VideoOverlay *pCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_VideoMaskConfig(VideoMaskConfig *pCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_VideoCaptureConfig(VideoCapture*pCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_VideoEncodeConfig(VideoEncode*pCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_AudioCaptureConfig(AudioCapture* audioCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_AudioEncodeConfig(AudioEncode *audioCfg);
	AJNETSDKDLL_API char*  AJ_NETSDK_IPC_XMLGET_VideoUserOverlayConfig(VideoUserOverlay *pCfg);
	AJNETSDKDLL_API LONG  AJ_NETSDK_IPC_XMLGET_SAFE_FREE(void* p);

#ifdef __cplusplus
}
#endif
#endif