
#include "Sal_Socket.h"
#include <string.h>
#include "../debug/exceptions.h"
#include "../utils/utils.h"


#ifdef _WIN32

#if WINVER <= 0x0502 // XP, Server2003
int hd_poll(HD_POLLFD* fdArray, ULONG nfds, INT timeout)
{
    fd_set read_fds, write_fds;
    fd_set* read_fds_ptr = NULL;
    fd_set* write_fds_ptr = NULL;

    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    for (ULONG i = 0; i < nfds; i++) {
        // Read (in) socket
        if ((fdArray[i].events & HD_POLLIN) == HD_POLLIN) {
            read_fds_ptr = &read_fds;
            FD_SET(fdArray[i].fd, &read_fds);
        }
        // Write (out) socket
        else if ((fdArray[i].events & HD_POLLOUT) == HD_POLLOUT) {
            write_fds_ptr = &write_fds;
            FD_SET(fdArray[i].fd, &write_fds);
        }
    }

    timeval time_out;
    timeval* time_out_ptr = NULL;
    if (timeout >= 0) {
        time_out.tv_sec = timeout / 1000;
        time_out.tv_usec = (timeout % 1000) * 1000;
        time_out_ptr = &time_out;
    }
    else { // to avoid compiler warnings
        (void)time_out;
        (void)timeout;
    }

    int sktready = select(1, read_fds_ptr, write_fds_ptr, NULL, time_out_ptr);
    if (sktready > 0) {
        for (ULONG i = 0; i < nfds; i++) {
            fdArray[i].revents = 0;
            if (FD_ISSET(fdArray[i].fd, &read_fds))
                fdArray[i].revents |= HD_POLLIN;
            if (FD_ISSET(fdArray[i].fd, &write_fds))
                fdArray[i].revents |= HD_POLLOUT;
        }
    }
    return sktready;
}
#else  // Vista, Win7...
int hd_poll(HD_POLLFD* fdArray, ULONG nfds, INT timeout) {
    return WSAPoll(fdArray, nfds, timeout);
}
#endif

int hd_fcntl(HD_SOCKET fd, int cmd, int flags)
{
    if (cmd != HD_F_GETFL && cmd != HD_F_SETFL) {
        return -1;
    }

    if (flags != HD_O_NONBLOCK && flags != 0) {
        return -1;
    }

    if (cmd == HD_F_GETFL) {
        return 0;
    }

    int ret = flags
        ? ioctlsocket(fd, FIONBIO, (u_long*)(&(flags = 1)))
        : ioctlsocket(fd, FIONBIO, (u_long*)(&(flags = 0)));

    return ret;
}
int hd_socketpair(int d, int type, int protocol, HD_SOCKET sv[2])
{
    UNUSED(d);
    UNUSED(type);
    UNUSED(protocol);

    if (sv == 0) {
        WSASetLastError(WSAEINVAL);
        return SOCKET_ERROR;
    }
    sv[0] = sv[1] = INVALID_SOCKET;

    HD_SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) {
        return SOCKET_ERROR;
    }

    sockaddr_in a;
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    a.sin_port = 0;
    socklen_t addrlen = sizeof(a);

    do {
        // ignore errors coming out of this setsockopt.  This is because
        // SO_EXCLUSIVEADDRUSE requires admin privileges on WinXP, but we don't
        // want to force socket pairs to be an admin.
        int reuse = 1;
        setsockopt(listener, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&reuse, (socklen_t)sizeof(reuse));
        if (bind(listener, (sockaddr*)&a, sizeof(a)) == SOCKET_ERROR) break;
        if (getsockname(listener, (sockaddr*)&a, &addrlen) == SOCKET_ERROR) break;
        if (listen(listener, 1) == SOCKET_ERROR) break;

        sv[0] = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
        if (sv[0] == INVALID_SOCKET) break;
        if (connect(sv[0], (sockaddr*)&a, sizeof(a)) == SOCKET_ERROR) break;

        sv[1] = accept(listener, NULL, NULL);
        if (sv[1] == INVALID_SOCKET) break;
        closesocket(listener);

        return 0;

    } while (0);

    int e = WSAGetLastError();
    closesocket(listener);
    closesocket(sv[0]);
    closesocket(sv[1]);
    WSASetLastError(e);

    return SOCKET_ERROR;
}

#endif


ECCS_BEGIN


//------------------------------------------------------
// SocketStartup
//------------------------------------------------------

#ifdef _WIN32
ECCS_C11 once_flag  SocketStartup::_flags;
SocketStartup* SocketStartup::_instance;
#endif

SocketStartup::SocketStartup()
{
#ifdef _WIN32
    WSADATA wsa;
    if (::WSAStartup(MAKEWORD(2, 2), &wsa)) {
        throw ESocketError("SocketStartup::SocketStartup()");
    }
#endif
}
SocketStartup::~SocketStartup()
{
#ifdef _WIN32
    ::WSACleanup();
#endif
}
void SocketStartup::startup()
{
#ifdef _WIN32
    ECCS_C11 call_once(_flags, startupImpl);
#endif
}
#ifdef _WIN32
void SocketStartup::startupImpl()
{
    _instance = new SocketStartup();
}
#endif


//------------------------------------------------------
// AddrInfoWrapper
//------------------------------------------------------

AddrInfoWrapper::AddrInfoWrapper(const str& host, int port, int sockType)
    : _node(host), _res(NULL)
{
    char sPort[32] = { 0 };
    sprintf(sPort, "%d", port);
    _service = sPort;

    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = PF_UNSPEC;
    _hints.ai_socktype = sockType;
    _hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
}
AddrInfoWrapper::AddrInfoWrapper(const str& node, const str& service, int sockType)
    : _node(node), _service(service), _res(NULL)
{
    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family = PF_UNSPEC;
    _hints.ai_socktype = sockType;
    _hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
}
AddrInfoWrapper::~AddrInfoWrapper()
{
    if (_res) {
        freeaddrinfo(_res);
        _res = NULL;
    }
}
int AddrInfoWrapper::init()
{
    SocketStartup::startup();

    int retval = 0;
    if (!_res)
    {
        // Windows:
        //   node=NULL: list ADDR_ANY of IPv4 and IPv6
        //   node=""  : list all availible interfaces (exclude ADDR_ANY)
        // Linux:
        //   node=NULL: list all availible interfaces or ADDR_ANY of IPv4 and IPv6
        //   node=""  : error
        const char* node = _node.empty() ? NULL : _node.c_str();
        retval = getaddrinfo(node, _service.c_str(), &_hints, &_res);
#ifdef _WIN32
        if (retval == WSANO_DATA) {
            _hints.ai_flags &= ~AI_ADDRCONFIG;
            retval = getaddrinfo(node, _service.c_str(), &_hints, &_res);
        }
#endif

#if 0
        // for debug
        printf("------------ getaddrinfo(%s:%s) ------------\n", _node.c_str(), _service.c_str());
        const addrinfo* ai = _res;
        for (; ai; ai = ai->ai_next) {
            printf("%d, %s\n", ai->ai_family, StrSockAddr(*(ai->ai_addr)).c_str());
        }
#endif
    }
    return retval;
}


//------------------------------------------------------
// Utilities
//------------------------------------------------------
/**
 * @brief IsValidPort 判断端口是否可用
 * @param port：需要判断的端口号
 * @return True：可用 ， False：不可用
 * @author：TJH
 */
bool IsValidPort(int port)
{
    return (port >= 0 && port <= 65535 && port != HD_INVALID_PORT);
}
/**
 * @brief IsValidSockAddr 判断地址是否可用
 * @param addr：需要判断的ip
 * @return True：可用 ， False：不可用
 * @author：TJH
 */
bool IsValidSockAddr(str addr)
{
    int iRet = -1;

    struct in_addr s4;
    iRet = inet_pton(AF_INET, addr.c_str(), &s4);
    if (iRet == 1) {
        return true;
    }

    struct in6_addr s6;
    iRet = inet_pton(AF_INET6, addr.c_str(), &s6);
    if (iRet == 1) {
        return true;
    }

    return false;
}
/**
 * @brief Equal 判断两个地址是否相同
 * @param addr0 ：待判断的地址1
 * @param addr1 ：待判断的地址2
 * @return True：相同，False：不同
 * @author：Tjh
 */
bool Equal(const sockaddr& addr0, const sockaddr& addr1)
{
    if (addr0.sa_family != addr1.sa_family) {
        return false;
    }

    switch (addr0.sa_family)
    {
    case AF_INET:
    {
        sockaddr_in* a0 = (sockaddr_in*)&addr0;
        sockaddr_in* a1 = (sockaddr_in*)&addr1;
        return a0->sin_addr.s_addr == a1->sin_addr.s_addr
            && a0->sin_port == a1->sin_port;
    }
    case AF_INET6:
    {
        sockaddr_in6* a0 = (sockaddr_in6*)&addr0;
        sockaddr_in6* a1 = (sockaddr_in6*)&addr1;
        if (a0->sin6_port != a1->sin6_port) {
            return false;
        }
        int iRet = memcmp(a0->sin6_addr.s6_addr, a1->sin6_addr.s6_addr, sizeof(a0->sin6_addr.s6_addr));
        if (iRet != 0) {
            return false;
        }
        return true;
    }
    default:
        return false;
    }
}
bool operator==(const sockaddr& addr0, const sockaddr& addr1)
{
    return Equal(addr0, addr1);
}
/**
 * @brief StrSockAddr ip地址输出成字符串
 * @param addr：待输出的ip地址
 * @return 输出字符串
 * @author：Tjh
 */
str StrSockAddr(const sockaddr& addr)
{
    char buf[128] = { 0 };
    switch (addr.sa_family)
    {
    case AF_INET:
    {
        sockaddr_in* a = (sockaddr_in*)&addr;
        snprintf(buf, sizeof(buf), "%s:%d", inet_ntoa(a->sin_addr), ntohs(a->sin_port));
        break;
    }
    case AF_INET6:
    {
        sockaddr_in6* a = (sockaddr_in6*)&addr;
        inet_ntop(AF_INET6, &(a->sin6_addr), buf, sizeof(buf));
        snprintf(buf, sizeof(buf), "%s:%d", buf, ntohs(a->sin6_port));
        break;
    }
    default:
        break;
    }
    return str(buf);
}


ECCS_END
