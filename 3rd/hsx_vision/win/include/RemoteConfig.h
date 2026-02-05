// UCRemoteConfig.h : UCRemoteConfig DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CUCRemoteConfigApp
// 有关此类实现的信息，请参阅 UCRemoteConfig.cpp
//

class CUCRemoteConfigApp : public CWinApp
{
public:
	CUCRemoteConfigApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


#define CONFIG_API extern "C" __declspec(dllexport) 

// pDeviceIP -- 设备IP， iPort -- 端口， pUser -- 用户名吗， pPassword --密码， pLan -- 语言， pParent -- 父窗口句柄，
// 返回lCOnfigID -- 配置标识（1-100）
CONFIG_API long CFG_UC_Init(char *pDeviceIP, int iPort, char *pUser, char *pPassword, char *pLan, LPVOID pParent);


// lCOnfigID -- 配置标识
CONFIG_API long CFG_UC_UnInit(long lConfigID);


// lCOnfigID -- 配置标识， rcPos -- 显示位置
// rcPos(0,0,0,0) 关闭窗口
CONFIG_API long CFG_UC_ShowModelessDlg(long lConfigID, RECT rcPos);

CONFIG_API long CFG_UC_ShowDlg(LPVOID pParent);
CONFIG_API long CFG_UC_CloseDlg();