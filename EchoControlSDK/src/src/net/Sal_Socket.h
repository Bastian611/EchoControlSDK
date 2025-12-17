
#pragma once

#include <memory>
#include "../global.h"
#include "../thread/sal_thread.h"


//------------------------------------------------------
// Macros for sockets
//------------------------------------------------------

#ifdef _WIN32
#  include <WinSock2.h>
#  include <WS2tcpip.h>
#  include <Windows.h>
#  pragma comment(lib, "ws2_32.lib")
#  define HD_GET_SOCKET_ERROR   ::WSAGetLastError()
#  define HD_ERRNO              (*_errno())
#  define HD_EINPROGRESS        WSAEINPROGRESS
#  define HD_EAGAIN             WSAEWOULDBLOCK
#  define HD_EINTR              WSAEINTR
#  define HD_ECONNRESET         WSAECONNRESET
#  define HD_ENOTCONN           WSAENOTCONN
#  define HD_ETIMEDOUT          WSAETIMEDOUT
#  define HD_EWOULDBLOCK        WSAEWOULDBLOCK
#  define HD_EPIPE              WSAECONNRESET
#  define HD_NO_SOCKET_CACHING  SO_EXCLUSIVEADDRUSE
#  define HD_SOCKET             SOCKET
#  define HD_INVALID_SOCKET     INVALID_SOCKET
#  define HD_INVALID_PORT       (-1)
#  define HD_GAI_STRERROR       gai_strerrorA
#  define HD_SOCKETPAIR  hd_socketpair
#  define HD_FCNTL       hd_fcntl
#  define HD_IOCTRL      ioctlsocket
#  define HD_O_NONBLOCK  1
#  define HD_F_GETFL     0
#  define HD_F_SETFL     1
#  define HD_CLOSESOCKET ::closesocket
#  define HD_SSIZET      ptrdiff_t
#  define HD_POLL        hd_poll
#  if WINVER <= 0x0502 //XP, Server2003
#    define HD_POLLFD    hd_pollfd
#    define HD_POLLIN    0x0300
#    define HD_POLLOUT   0x0010
#  else                //Vista, Win7...
#    define HD_POLLFD    pollfd
#    define HD_POLLIN    POLLIN
#    define HD_POLLOUT   POLLOUT
#  endif
#  define HD_SHUT_RDWR  SD_BOTH
#else
#  include <errno.h>
#  include <unistd.h>
#  include <ifaddrs.h>
#  include <netdb.h>
#  include <poll.h>
#  include <fcntl.h>
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <sys/ioctl.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#  include <arpa/inet.h>
#  define HD_GET_SOCKET_ERROR  errno
#  define HD_ERRNO             errno
#  define HD_EINTR             EINTR
#  define HD_EINPROGRESS       EINPROGRESS
#  define HD_ECONNRESET        ECONNRESET
#  define HD_ENOTCONN          ENOTCONN
#  define HD_ETIMEDOUT         ETIMEDOUT
#  define HD_EWOULDBLOCK       EWOULDBLOCK
#  define HD_EAGAIN            EAGAIN
#  define HD_EPIPE             EPIPE
#  define HD_NO_SOCKET_CACHING SO_REUSEADDR
#  define HD_SOCKET            int
#  define HD_INVALID_SOCKET    (-1)
#  define HD_INVALID_PORT      (-1)
#  define HD_GAI_STRERROR gai_strerror
#  define HD_SOCKETPAIR   socketpair
#  define HD_FCNTL        fcntl
#  define HD_IOCTRL       ioctl
#  define HD_O_NONBLOCK   O_NONBLOCK
#  define HD_F_GETFL      F_GETFL
#  define HD_F_SETFL      F_SETFL
#  define HD_CLOSESOCKET  ::close
#  define HD_SSIZET       ssize_t
#  define HD_POLL         poll
#  define HD_POLLFD       pollfd
#  define HD_POLLIN       POLLIN
#  define HD_POLLOUT      POLLOUT
#  define HD_SHUT_RDWR    SHUT_RDWR
#endif


#ifdef _WIN32

#if WINVER <= 0x0502 // XP, Server2003
struct hd_pollfd
{
    HD_SOCKET fd;
    SHORT events;
    SHORT revents;
};
#endif

int hd_poll(HD_POLLFD* fdArray, ULONG nfds, INT timeout);
int hd_fcntl(HD_SOCKET fd, int cmd, int flags);
int hd_socketpair(int d, int type, int protocol, HD_SOCKET sv[2]);

#endif


ECCS_BEGIN


//------------------------------------------------------
// SocketStartup
//------------------------------------------------------

class SocketStartup
{
private:
    NON_COPYABLE(SocketStartup);
    SocketStartup();

public:
    ~SocketStartup();
    static void startup();

#ifdef _WIN32
private:
    static void startupImpl();
    static ECCS_C11 once_flag  _flags;
    static SocketStartup* _instance;
#endif
};


//------------------------------------------------------
// AddrInfoWrapper
//------------------------------------------------------

class AddrInfoWrapper
{
    NON_COPYABLE(AddrInfoWrapper);

public:
    AddrInfoWrapper(const str& host, int port, int sockType);
    AddrInfoWrapper(const str& node, const str& service, int sockType);
    ~AddrInfoWrapper();

    int init();
    addrinfo* hints() { return &_hints; }
    const addrinfo* res() const { return _res; }

private:
    str        _node;
    str        _service;
    addrinfo   _hints;
    addrinfo* _res;
};


//------------------------------------------------------
// Utilities
//------------------------------------------------------
/**
 * @brief IsValidPort 判断端口是否可用
 * @param port：需要判断的端口号
 * @return True：可用 ， False：不可用
 */
bool IsValidPort(int port);
/**
 * @brief IsValidSockAddr 判断地址是否可用
 * @param addr：需要判断的ip
 * @return True：可用 ， False：不可用
 */
bool IsValidSockAddr(str addr);
/**
 * @brief Equal 判断两个地址是否相同
 * @param addr0 ：待判断的地址1
 * @param addr1 ：待判断的地址2
 * @return True：相同，False：不同
 */
bool Equal(const sockaddr& addr0, const sockaddr& addr1);
bool operator==(const sockaddr& addr0, const sockaddr& addr1);
/**
 * @brief StrSockAddr ip地址输出成字符串
 * @param addr：待输出的ip地址
 * @return 输出字符串
 */
str StrSockAddr(const sockaddr& addr);


ECCS_END
