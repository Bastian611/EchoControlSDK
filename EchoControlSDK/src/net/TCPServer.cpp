
#include "tcpserver.h"
#include <string.h>
#include "../debug/exceptions.h"
#include "../utils/utils.h"
#include "../time/time_utils.h"

#ifndef AF_LOCAL
#define AF_LOCAL AF_UNIX
#endif

ECCS_BEGIN


static void destroyerSocket(HD_SOCKET* sock)
{
    HD_CLOSESOCKET(*sock);
    delete sock;
}


TcpServer::TcpServer(int port)
{
    _port = port;
    _portBind = _port;
    _acceptBacklog = 64;
    _sendTimeout = 0;
    _recvTimeout = 0;
    _accTimeout = -1;
    _retryLimit = 3;
    _retryDelay = 0;
    _sendBufSize = 0;
    _recvBufSize = 0;
    _keepAlive = false;
    _listening = false;
    _interruptableChildren = true;

    _serverSock = HD_INVALID_SOCKET;
    _interruptWriter = HD_INVALID_SOCKET;
    _interruptReader = HD_INVALID_SOCKET;
    _interrupChildWriter = HD_INVALID_SOCKET;

    memset(&_cachedLocalAddr, 0, sizeof(_cachedLocalAddr));
    _cachedLocalAddr.ipv4.sin_family = AF_UNSPEC;
}
TcpServer::TcpServer(str addr, int port)
{
    _port = port;
    _portBind = _port;
    _addr = addr;
    _acceptBacklog = 64;
    _sendTimeout = 0;
    _recvTimeout = 0;
    _accTimeout = -1;
    _retryLimit = 3;
    _retryDelay = 0;
    _sendBufSize = 0;
    _recvBufSize = 0;
    _keepAlive = false;
    _listening = false;
    _interruptableChildren = true;

    _serverSock = HD_INVALID_SOCKET;
    _interruptWriter = HD_INVALID_SOCKET;
    _interruptReader = HD_INVALID_SOCKET;
    _interrupChildWriter = HD_INVALID_SOCKET;

    memset(&_cachedLocalAddr, 0, sizeof(_cachedLocalAddr));
    _cachedLocalAddr.ipv4.sin_family = AF_UNSPEC;
}
TcpServer::~TcpServer()
{
    close();
}

sockaddr* TcpServer::cachedLocalAddress(socklen_t* len) const
{
    switch (_cachedLocalAddr.ipv4.sin_family)
    {
    case AF_INET:
        if (len){
            *len = sizeof(sockaddr_in);
        }
        return (sockaddr*)&_cachedLocalAddr.ipv4;
    case AF_INET6:
        if (len){
            *len = sizeof(sockaddr_in6);
        }
        return (sockaddr*)&_cachedLocalAddr.ipv6;
    default:
        return NULL;
    }
}
void TcpServer::setCachedLocalAddress(const sockaddr* addr, socklen_t len)
{
    switch(addr->sa_family)
    {
    case AF_INET:
        if (len == sizeof(sockaddr_in)) {
            memcpy((void*)&_cachedLocalAddr.ipv4, (void*)addr, len);
        }
        break;
    case AF_INET6:
        if (len == sizeof(sockaddr_in6)) {
            memcpy((void*)&_cachedLocalAddr.ipv6, (void*)addr, len);
        }
        break;
    default:
        break;
    }
}

void TcpServer::setSendTimeout(int to)
{
    _sendTimeout = to;
}
void TcpServer::setRecvTimeout(int to)
{
    _recvTimeout = to;
}
void TcpServer::setKeepAlive(bool keepAlive)
{
    _keepAlive = keepAlive;
}

void TcpServer::setAcceptTimeout(int to)
{
    _accTimeout = to;
}
void TcpServer::setAcceptBacklog(int backlog)
{
    _acceptBacklog = backlog;
}
void TcpServer::setRetryLimit(int limit)
{
    _retryLimit = limit;
}
void TcpServer::setRetryDelay(int delay)
{
    _retryDelay = delay;
}
void TcpServer::setSendBuffer(int len)
{
    _sendBufSize = len;
}
void TcpServer::setRecvBuffer(int len)
{
    _recvBufSize = len;
}
void TcpServer::setInterruptableChildren(bool enable)
{
    if (_listening){
        throw EInvalidOperation("TcpServer::setInterruptableChildren() cannot be called after listen()");
    }
    _interruptableChildren = enable;
}

void TcpServer::listen()
{
    // Validate
    if (_listening || _serverSock != HD_INVALID_SOCKET){
        throw EInvalidOperation("TcpServer::listen() already listenning");
    }
    if (!IsValidPort(_port)) {
        throw EInvalidParam("TcpServer::listen() invalid TCP port: %d", _port);
    }

    SocketStartup::startup();
    _listening = true;

    // Create the socket pair used to interrupt
    HD_SOCKET sv[2];
    if (HD_SOCKETPAIR(AF_LOCAL, SOCK_STREAM, 0, sv) == -1) {
        _interruptWriter = HD_INVALID_SOCKET;
        _interruptReader = HD_INVALID_SOCKET;
    }
    else {
        _interruptWriter = sv[1];
        _interruptReader = sv[0];
    }

    // Create the socket pair used to interrupt all clients
    if (HD_SOCKETPAIR(AF_LOCAL, SOCK_STREAM, 0, sv) == -1) {
        _interrupChildWriter = HD_INVALID_SOCKET;
        _interruptChildReader.reset();
    }
    else {
        _interrupChildWriter = sv[1];
        _interruptChildReader = std::shared_ptr<HD_SOCKET>(new HD_SOCKET(sv[0]), destroyerSocket);
    }

    // Get address info
    AddrInfoWrapper addrInfo(_addr, _port, SOCK_STREAM);
    int iRet = addrInfo.init();
    if (iRet){
        close();
        str eStr = "TcpServer::listen() getaddrinfo()" + str(HD_GAI_STRERROR(iRet));
        throw ESocketError(eStr.c_str());
    }

    // Pick the ipv6 address first since ipv4 addresses can be mapped
    // into ipv6 space.
    // If multi network card in computer and _addr is empty, pick
    // one of the addresses to be bind.
    const addrinfo* res = addrInfo.res();
    for (; res; res = res->ai_next) {
        if (res->ai_family == AF_INET6 || res->ai_next == NULL){
            break;
        }
    }

    _serverSock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (_serverSock == HD_INVALID_SOCKET){
        int e = HD_GET_SOCKET_ERROR;
        close();
        throw ESocketError("TcpServer::listen() socket()", e);
    }

    int one = 1;
#ifdef _WIN32
    // Turn on SO_REUSEADDR for security
    // Ignore errors coming out of this setsockopt on Windows. This is because
    // SO_EXCLUSIVEADDRUSE requires admin privileges on WinXP, but we don't
    // want to force servers to be an admin.
    setsockopt(_serverSock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char*)(&one), sizeof(one));
#else
    // Turn on SO_REUSEADDR for avoiding bind() error when restart TcpServer
    if (setsockopt(_serverSock, SOL_SOCKET, SO_REUSEADDR, (const char*)(&one), sizeof(one)) == -1) {
        int e = HD_GET_SOCKET_ERROR;
        close();
        throw ESocketError("TcpServer::listen() setsockopt() SO_REUSEADDR", e);
    }
#endif

    // Set buffer size
    if (_sendBufSize > 0){
        if (setsockopt(_serverSock, SOL_SOCKET, SO_SNDBUF, (const char*)(&_sendBufSize), sizeof(_sendBufSize)) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            close();
            throw ESocketError("TcpServer::listen() setsockopt() ", e);
        }
    }
    if (_recvBufSize > 0){
        if (setsockopt(_serverSock, SOL_SOCKET, SO_RCVBUF, (const char*)(&_recvBufSize), sizeof(_recvBufSize)) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            close();
            throw ESocketError("TcpServer::listen() () ", e);
        }
    }

#ifdef TCP_DEFER_ACCEPT
    if (setsockopt(_serverSock, IPPROTO_TCP, TCP_DEFER_ACCEPT, &one, sizeof(one)) == -1) {
        int e = HD_GET_SOCKET_ERROR;
        close();
        throw ESocketError("TcpServer::listen() setsockopt() TCP_DEFER_ACCEPT", e);
    }
#endif

#ifdef IPV6_V6ONLY
    if (res->ai_family == AF_INET6) {
        int zero = 0;
        setsockopt(_serverSock, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)(&zero), sizeof(zero));
    }
#endif

    // Turn linger off, don't want to block on calls to close
    linger l = {0, 0};
    if (setsockopt(_serverSock, SOL_SOCKET, SO_LINGER, (const char*)(&l), sizeof(l)) == -1) {
        int e = HD_GET_SOCKET_ERROR;
        close();
        throw ESocketError("TcpServer::listen() setsockopt() SO_LINGER", e);
    }

    // Turn on NODELAY
    if (setsockopt(_serverSock, IPPROTO_TCP, TCP_NODELAY, (const char*)(&one), sizeof(one)) == -1) {
        int e = HD_GET_SOCKET_ERROR;
        close();
        throw ESocketError("TcpServer::listen() setsockopt() TCP_NODELAY", e);
    }

    // Set NONBLOCK on the accept socket
    int flags = HD_FCNTL(_serverSock, HD_F_GETFL, 0);
    if (flags == -1) {
        int e = HD_GET_SOCKET_ERROR;
        close();
        throw ESocketError("TcpServer::listen() HD_FCNTL() HD_F_GETFL", e);
    }
    if (HD_FCNTL(_serverSock, HD_F_SETFL, flags | HD_O_NONBLOCK) == -1) {
        int e = HD_GET_SOCKET_ERROR;
        close();
        throw ESocketError("TcpServer::listen() HD_FCNTL() HD_F_SETFL", e);
    }

    // prepare the port information
    // we may want to try to bind more than once, since SO_REUSEADDR doesn't
    // always seem to work.
    int retries = 0, e = 0;
    while(++retries <= _retryLimit){
        if (::bind(_serverSock, res->ai_addr, int(res->ai_addrlen)) == 0) {
            break;
        }
        e = HD_GET_SOCKET_ERROR;
        msleep(0);
    }

    // retrieve bind info
    if (_port == 0 && retries <= _retryLimit)
    {
        sockaddr_storage sa;
        socklen_t len = sizeof(sa);
        memset(&sa, 0, len);
        if (::getsockname(_serverSock, (sockaddr*)(&sa), &len) < 0) {
            e = HD_GET_SOCKET_ERROR;
            close();
            throw ESocketError("TcpServer::listen() getsockname()", e);
        }

        if (sa.ss_family == AF_INET6) {
            const sockaddr_in6* sin = (const sockaddr_in6*)(&sa);
            _portBind = ntohs(sin->sin6_port);
        }
        else {
            const sockaddr_in* sin = (const sockaddr_in*)(&sa);
            _portBind = ntohs(sin->sin_port);
        }
    }

    // failed to bind
    if (retries > _retryLimit) {
        char ebuf[1024];
        snprintf(ebuf, sizeof(ebuf), "TcpServer::listen() BIND %d", _port);
        close();
        throw ESocketError(ebuf, e);
    }

    // Callback
    if (_listenCallback){
        _listenCallback(_serverSock);
    }

    // Call listen
    if (::listen(_serverSock, _acceptBacklog) == -1) {
        e = HD_GET_SOCKET_ERROR;
        close();
        throw ESocketError("TcpServer::listen() listen()", e);
    }

    // The socket is now listening!

    setCachedLocalAddress(res->ai_addr, (socklen_t)res->ai_addrlen);
}
TcpSocket_Ptr TcpServer::accept()
{
    if (!_listening || _serverSock == HD_INVALID_SOCKET){
        throw EInvalidOperation("TcpServer::accept() socket require listenning or socket invalid");
    }

    if (_accTimeout != 0)
    {
        // Wait for incoming request
        int maxEintrs = 5;
        int numEintrs = 0;
        HD_POLLFD fds[2];
        while (true)
        {
            memset(fds, 0, sizeof(fds));
            fds[0].fd = _serverSock;
            fds[0].events = HD_POLLIN;
            if (_interruptReader != HD_INVALID_SOCKET) {
                fds[1].fd = _interruptReader;
                fds[1].events = HD_POLLIN;
            }

            int iRet = HD_POLL(fds, 2, _accTimeout);
            if (iRet < 0) {
                // error cases
                int e = HD_GET_SOCKET_ERROR;
                if (e == HD_EINTR && (++numEintrs <= maxEintrs)) {
                    // HD_EINTR needs to be handled manually and we can tolerate a certain number
                    continue;
                }
                throw ESocketError("TcpServer::accept() HD_POLL()", e);
            }
            else if (iRet > 0) {
                // Check for an interrupt signal
                if (_interruptReader != HD_INVALID_SOCKET && (fds[1].revents & HD_POLLIN)) {
                    i8 buf;
                    recv(_interruptReader, (char*)(&buf), sizeof(i8), 0);
                    throw EInterrupt("TcpServer::accept() interrupt");
                }
                // Check for the actual server socket being ready
                if (fds[0].revents & HD_POLLIN) {
                    break;
                }
            }
            else {
                // Timeout
                throw ETimeout("TcpServer::accept() HD_POLL()");
            }
        }
    }

    // Accept one
    sockaddr_storage clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    HD_SOCKET clientSock = ::accept(_serverSock, (sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSock == HD_INVALID_SOCKET)
    {
        int e = HD_GET_SOCKET_ERROR;
        if (_accTimeout == 0) {
            // !! set _serverSock to NONBLOCK in listen()
            if (e == HD_EAGAIN || e == HD_EWOULDBLOCK) {
                return TcpSocket_Ptr();
            }
        }
        throw ESocketError("TcpServer::accept() accept()", e);
    }

    // Make sure client socket is blocking
    int flags = HD_FCNTL(clientSock, HD_F_GETFL, 0);
    if (flags == -1) {
        int e = HD_GET_SOCKET_ERROR;
        HD_CLOSESOCKET(clientSock);
        throw ESocketError("TcpServer::accept() HD_FCNTL() HD_F_GETFL", e);
    }
    if (HD_FCNTL(clientSock, HD_F_SETFL, flags & ~HD_O_NONBLOCK) == -1) {
        int e = HD_GET_SOCKET_ERROR;
        HD_CLOSESOCKET(clientSock);
        throw ESocketError("TcpServer::accept() HD_FCNTL() HD_F_SETFL", e);
    }

    // Create a TcpSocket
    std::shared_ptr<TcpSocket> retSock(new TcpSocket(clientSock, _interruptableChildren ? _interruptChildReader : NULL));

    if (_sendTimeout > 0){
        retSock->setSendTimeout(_sendTimeout);
    }
    if (_recvTimeout > 0){
        retSock->setRecvTimeout(_recvTimeout);
    }
    if (_keepAlive){
        retSock->setKeepAlive(_keepAlive);
    }
    retSock->setCachedPeerAddress((sockaddr*)&clientAddr, clientAddrLen);

    // Callback
    if (_acceptCallback){
        _acceptCallback(clientSock);
    }

    return retSock;
}
void TcpServer::interrupt()
{
    notify(_interruptWriter);
}
void TcpServer::interruptChildren()
{
    notify(_interrupChildWriter);
}
void TcpServer::close()
{
    if (_serverSock != HD_INVALID_SOCKET) {
        HD_CLOSESOCKET(_serverSock);
    }
    if (_interruptWriter != HD_INVALID_SOCKET) {
        HD_CLOSESOCKET(_interruptWriter);
    }
    if (_interruptReader != HD_INVALID_SOCKET) {
        HD_CLOSESOCKET(_interruptReader);
    }
    if (_interrupChildWriter != HD_INVALID_SOCKET) {
        HD_CLOSESOCKET(_interrupChildWriter);
    }
    _serverSock = HD_INVALID_SOCKET;
    _interruptWriter = HD_INVALID_SOCKET;
    _interruptReader = HD_INVALID_SOCKET;
    _interrupChildWriter = HD_INVALID_SOCKET;
    _cachedLocalAddr.ipv4.sin_family = AF_UNSPEC;
    _interruptChildReader.reset();
    _listening = false;
}
void TcpServer::notify(HD_SOCKET sock)
{
    if (sock != HD_INVALID_SOCKET) {
        i8 byte = 0;
        if (-1 == send(sock, (const char*)(&byte), sizeof(i8), 0)) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("TcpServer::notify() send()", e);
        }
    }
}


ECCS_END
