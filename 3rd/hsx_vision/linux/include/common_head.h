#ifndef __COMMON_HEAD_H
#define __COMMON_HEAD_H

#ifndef UINT
#define UINT         unsigned int
#endif
#ifndef LONG
#define LONG         long
#endif
#ifndef ULONG
#define ULONG        unsigned long
#endif
#ifndef DWORD
#define DWORD        unsigned long
#endif
#ifndef WORD
#define WORD         unsigned short
#endif
#ifndef FOURCC
#define FOURCC       unsigned long
#endif
#ifndef DOUBLE
#define DOUBLE       double
#endif
#ifndef CALLBACK
#define CALLBACK     
#endif
#ifndef HWND
#define HWND         int
#endif
#ifndef BOOL
#define BOOL         int
#endif
#ifndef INT
#define INT          int
#endif
#ifndef BYTE
#define BYTE         char
#endif
#ifndef PBYTE
#define PBYTE        char *
#endif
#ifndef LPVOID
#define LPVOID       void *
#endif
#ifndef LPDWORD
#define LPDWORD      unsigned long*
#endif
#ifndef SOCKET
#define SOCKET       int 
#endif
#ifndef HANDLE
#define HANDLE       int 
#endif
#ifndef FILETIME
#define FILETIME     unsigned int
#endif
#ifndef _bstr_t
#define _bstr_t      char *   //string
#endif
#ifndef CHAR
#define CHAR         char
#endif
#ifndef TCHAR
#define TCHAR        char
#endif
#ifndef WPARAM
#define WPARAM       void *
#endif
#ifndef LPARAM
#define LPARAM       void *
#endif
#ifndef TRUE
#define TRUE         1
#endif
#ifndef FALSE
#define FALSE        0
#endif

#ifndef CRITICAL_SECTION
#define CRITICAL_SECTION     pthread_mutex_t
#endif
#ifndef NETSDKDLL_API
#define NETSDKDLL_API
#endif
#ifndef __stdcall
#define __stdcall
#endif
#ifndef WINAPI
#define WINAPI    
#endif
#ifndef DLLPLAYER_API
#define DLLPLAYER_API
#endif

#define Sleep(time)        usleep(1000*(time))
//typedef _T(const char* str)    str ;

#define		AV_FALG		"AnjvsionH264"	//avhd

#define		ALLOW_LOGIN_MORETHAN_ONE			1

//2G
#define		AVI_RECORD_RESERVED_FREE_SPACE	2048

#define MAX_THREAD_STACK_SIZE_NORMAL (1*1024*1024)//2M

#define MAX_NAME_LEN 32

#endif


