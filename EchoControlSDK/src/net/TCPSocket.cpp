
#include "TCPSocket.h"
#include <sstream>
#include <string.h>
#include "../global.h"

#include "../debug/exceptions.h"
#include "../debug/str_error.h"

#include "../time/elapsed_timer.h"
#include "../time/time_utils.h"
#include "../time/sal_chrono.h"

#include "../utils/utils.h"
#include "../utils/buffer.h"
#include "../utils/crc.h"
#include "../utils/projection.h"
#include "../utils/singleton.hpp"
#include "../utils/factory.hpp"
#include "../utils/file_system.h"

ECCS_BEGIN


TcpSocket::TcpSocket(str host, int port) : _host(host), _port(port)
{
    _localPort = HD_INVALID_PORT;
    _localPortBind = HD_INVALID_PORT;
    _peerPort = HD_INVALID_PORT;

    _connTimeout = 0;
    _recvTimeout = 0;
    _sendTimeout = 0;
    _sendBufSize = 0;
    _recvBufSize = 0;
    _maxRetries = 5;
    _keepAlive = false;
    _noDelay = true;

    _lingerVal = 0;
    _lingerOn = true;

    _sock = HD_INVALID_SOCKET;
    memset(&_cachedLocalAddr, 0, sizeof(_cachedLocalAddr));
    memset(&_cachedPeerAddr, 0, sizeof(_cachedPeerAddr));
    _cachedLocalAddr.ipv4.sin_family = AF_UNSPEC;
    _cachedPeerAddr.ipv4.sin_family = AF_UNSPEC;
}
TcpSocket::TcpSocket(HD_SOCKET sock, std::shared_ptr<HD_SOCKET> interrupt)
{
    _port = HD_INVALID_PORT;
    _localPort = HD_INVALID_PORT;
    _localPortBind = HD_INVALID_PORT;
    _peerPort = HD_INVALID_PORT;

    _connTimeout = 0;
    _recvTimeout = 0;
    _sendTimeout = 0;
    _sendBufSize = 0;
    _recvBufSize = 0;
    _maxRetries = 5;
    _keepAlive = false;
    _noDelay = true;

    _lingerVal = 0;
    _lingerOn = true;

    _sock = sock;
    _interrupt = interrupt;
#ifdef SO_NOSIGPIPE
    {
        int one = 1;
        setsockopt(_sock, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof(one));
    }
#endif

    memset(&_cachedLocalAddr, 0, sizeof(_cachedLocalAddr));
    memset(&_cachedPeerAddr, 0, sizeof(_cachedPeerAddr));
    _cachedLocalAddr.ipv4.sin_family = AF_UNSPEC;
    _cachedPeerAddr.ipv4.sin_family = AF_UNSPEC;
}
TcpSocket::~TcpSocket()
{
    close();
}

str TcpSocket::host() const
{
    return _host;
}
int TcpSocket::port() const
{
    return _port;
}
str TcpSocket::socketInfo()
{
    std::ostringstream oss;
    if (_host.empty() || _port == HD_INVALID_PORT) {
        oss << "<Host: " << peerAddress();
        oss << " Port: " << peerPort() << ">";
    }
    else {
        oss << "<Host: " << _host << " Port: " << _port << ">";
    }
    return oss.str();
}
HD_SOCKET TcpSocket::socket()
{
    return _sock;
}

str TcpSocket::localHost() const
{
    return _localHost;
}
int TcpSocket::localPort() const
{
    return _localPortBind;
}
void TcpSocket::setLocalAddr(str host, int port)
{
    if (isOpen()) {
        throw EInvalidOperation("setLocalAddr() should be call before open()");
    }
    _localHost = host;
    _localPort = port;
}

str TcpSocket::peerHost()
{
    if (_peerHost.empty())
    {
        if (_sock == HD_INVALID_SOCKET) {
            return _host;
        }

        socklen_t addrLen;
        sockaddr* pAddr = cachedPeerAddress(&addrLen);
        if (!pAddr) {
            sockaddr_storage addr;
            addrLen = sizeof(addr);
            if (getpeername(_sock, (sockaddr*)&addr, &addrLen) != 0) {
                return _peerHost;  // empty
            }
            pAddr = (sockaddr*)&addr;
            setCachedPeerAddress(pAddr, addrLen);
        }

        char clientHost[NI_MAXHOST] = { 0 };
        char clientService[NI_MAXSERV] = { 0 };
        getnameinfo((sockaddr*)pAddr,
            addrLen,
            clientHost,
            sizeof(clientHost),
            clientService,
            sizeof(clientService),
            0);
        _peerHost = clientHost;
    }
    return _peerHost;
}
str TcpSocket::peerAddress()
{
    if (_peerAddr.empty())
    {
        if (_sock == HD_INVALID_SOCKET) {
            return _peerAddr;
        }

        socklen_t addrLen;
        sockaddr* pAddr = cachedPeerAddress(&addrLen);
        if (!pAddr) {
            sockaddr_storage addr;
            addrLen = sizeof(addr);
            if (getpeername(_sock, (sockaddr*)&addr, &addrLen) != 0) {
                return _peerAddr;
            }
            pAddr = (sockaddr*)&addr;
            setCachedPeerAddress(pAddr, addrLen);
        }

        char clientHost[NI_MAXHOST] = { 0 };
        char clientService[NI_MAXSERV] = { 0 };
        getnameinfo(pAddr,
            addrLen,
            clientHost,
            sizeof(clientHost),
            clientService,
            sizeof(clientService),
            NI_NUMERICHOST | NI_NUMERICSERV);

        _peerAddr = clientHost;
        _peerPort = atoi(clientService);
    }
    return _peerAddr;
}
int TcpSocket::peerPort()
{
    peerAddress();
    return _peerPort;
}
str TcpSocket::origin()
{
    std::ostringstream oss;
    oss << peerAddress() << ":" << peerPort();
    return oss.str();
}

sockaddr* TcpSocket::cachedLocalAddress(socklen_t* len) const
{
    switch (_cachedLocalAddr.ipv4.sin_family)
    {
    case AF_INET:
        if (len) {
            *len = sizeof(sockaddr_in);
        }
        return (sockaddr*)&_cachedLocalAddr.ipv4;
    case AF_INET6:
        if (len) {
            *len = sizeof(sockaddr_in6);
        }
        return (sockaddr*)&_cachedLocalAddr.ipv6;
    default:
        return NULL;
    }
}
sockaddr* TcpSocket::cachedPeerAddress(socklen_t* len) const
{
    switch (_cachedPeerAddr.ipv4.sin_family)
    {
    case AF_INET:
        if (len) {
            *len = sizeof(sockaddr_in);
        }
        return (sockaddr*)&_cachedPeerAddr.ipv4;
    case AF_INET6:
        if (len) {
            *len = sizeof(sockaddr_in6);
        }
        return (sockaddr*)&_cachedPeerAddr.ipv6;
    default:
        return NULL;
    }
}
void TcpSocket::setCachedLocalAddress(const sockaddr* addr, socklen_t len)
{
    switch (addr->sa_family)
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
void TcpSocket::setCachedPeerAddress(const sockaddr* addr, socklen_t len)
{
    switch (addr->sa_family)
    {
    case AF_INET:
        if (len == sizeof(sockaddr_in)) {
            memcpy((void*)&_cachedPeerAddr.ipv4, (void*)addr, len);
        }
        break;
    case AF_INET6:
        if (len == sizeof(sockaddr_in6)) {
            memcpy((void*)&_cachedPeerAddr.ipv6, (void*)addr, len);
        }
        break;
    default:
        break;
    }
    _peerAddr.clear();
    _peerHost.clear();
}

bool TcpSocket::isOpen() const
{
    return (_sock != HD_INVALID_SOCKET);
}
void TcpSocket::open()
{
    if (isOpen()) {
        return;
    }
    if (!IsValidPort(_port)) {
        throw EInvalidParam("TcpSocket::open() invalid TCP port: %d", _port);
    }
    SocketStartup::startup();

    // get server address info
    AddrInfoWrapper addrInfo(_host.c_str(), _port, SOCK_STREAM);
    int iRet = addrInfo.init();
    if (iRet) {
        str eStr = "getaddrinfo(): " + socketInfo() + HD_GAI_STRERROR(iRet);
        throw ESocketError(eStr.c_str());
    }

    // Cycle through all the returned addresses until one
    // connects or push the exception up.
    const addrinfo* res = addrInfo.res();
    for (; res; res = res->ai_next) {
        try { open(res); break; }
        catch (...) {
            close();
            if (!(res->ai_next)) throw;
        }
    }
}
void TcpSocket::open(const addrinfo* addr)
{
    _sock = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (_sock == HD_INVALID_SOCKET) {
        int e = HD_GET_SOCKET_ERROR;
        throw ESocketError("TcpSocket::open() socket()", e);
    }

    if (_sendTimeout > 0) {
        setSendTimeout(_sendTimeout);
    }
    if (_recvTimeout > 0) {
        setRecvTimeout(_recvTimeout);
    }
    if (_sendBufSize > 0) {
        setSendBufSize(_sendBufSize);
    }
    if (_recvBufSize > 0) {
        setRecvBufSize(_recvBufSize);
    }
    if (_keepAlive) {
        setKeepAlive(_keepAlive);
    }
    setLinger(_lingerOn, _lingerVal);
    setNoDelay(_noDelay);

#ifdef SO_NOSIGPIPE
    {
        int one = 1;
        setsockopt(socket_, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof(one));
    }
#endif

    // bind local port if need
    if (IsValidPort(_localPort))
    {
        // get local address info
        AddrInfoWrapper addrInfo(_localHost.c_str(), _localPort, SOCK_STREAM);
        int iRet = addrInfo.init();
        if (iRet) {
            str eStr = "getaddrinfo(): " + socketInfo() + HD_GAI_STRERROR(iRet);
            throw ESocketError(eStr.c_str());
        }

        // try bind local port
        bool bound = false;
        const addrinfo* res = addrInfo.res();
        for (; res; res = res->ai_next)
        {
            if (res->ai_family != addr->ai_family) {
                continue;
            }

            try {
                bind(res);
                bound = true;
                break;
            }
            catch (ESocketError&) {
                if (!(res->ai_next)) throw;
            }
        }

        // failed to bind
        if (!bound) {
            char ebuf[1024];
            snprintf(ebuf, sizeof(ebuf), "TcpSocket::open() BIND %d", _localPort);
            throw ESocketError(ebuf);
        }
    }

    // Set the socket to be non blocking for connect if a timeout exists
    int flags = HD_FCNTL(_sock, HD_F_GETFL, 0);
    if (_connTimeout > 0) {
        if (HD_FCNTL(_sock, HD_F_SETFL, flags | HD_O_NONBLOCK) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("TcpSocket::open() HD_FCNTL()", e);
        }
    }
    else {
        if (HD_FCNTL(_sock, HD_F_SETFL, flags & ~HD_O_NONBLOCK) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("TcpSocket::open() HD_FCNTL()", e);
        }
    }

    // Connect the socket
    int iRet = connect(_sock, addr->ai_addr, int(addr->ai_addrlen));
    if (iRet)
    {
        int e = HD_GET_SOCKET_ERROR;
        if ((e != HD_EINPROGRESS) && (e != HD_EWOULDBLOCK)) {
            throw ESocketError("TcpSocket::open() connect()", e);
        }

        struct HD_POLLFD fds[1];
        memset(fds, 0, sizeof(fds));
        fds[0].fd = _sock;
        fds[0].events = HD_POLLOUT;
        iRet = HD_POLL(fds, 1, _connTimeout);

        if (iRet > 0)
        {
            // Ensure the socket is connected and that there are no errors set
            int val;
            socklen_t len = sizeof(int);
            if (getsockopt(_sock, SOL_SOCKET, SO_ERROR, (char*)(&val), &len) == -1) {
                int e = HD_GET_SOCKET_ERROR;
                throw ESocketError("TcpSocket::open() getsockopt()", e);
            }
            if (val != 0) {
                throw ESocketError("TcpSocket::open() getsockopt() SO_ERROR");
            }
        }
        else if (iRet == 0) {
            // socket timed out
            throw ESocketError("TcpSocket::open() HD_POLL() timed out");
        }
        else {
            // error on HD_POLL()
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("TcpSocket::open() HD_POLL()", e);
        }
    }

    // Set socket back to normal mode (blocking)
    HD_FCNTL(_sock, HD_F_SETFL, flags);

    // Get local socket address
    sockaddr_storage sa;
    socklen_t len = sizeof(sa);
    memset(&sa, 0, len);
    if (::getsockname(_sock, (sockaddr*)(&sa), &len) < 0) {
        int e = HD_GET_SOCKET_ERROR;
        throw ESocketError("TcpSocket::open() getsockname()", e);
    }

    setCachedLocalAddress((sockaddr*)(&sa), len);
    setCachedPeerAddress(addr->ai_addr, (socklen_t)(addr->ai_addrlen));
}
void TcpSocket::bind(const addrinfo* addr)
{
    if (_sock == HD_INVALID_SOCKET) {
        throw EInvalidParam("TcpSocket::bind() invalid socket");
    }

    // prepare the port information
    // we may want to try to bind more than once, since SO_REUSEADDR doesn't
    // always seem to work.
    int retries = 0, e = 0;
    while (++retries <= _maxRetries) {
        if (::bind(_sock, addr->ai_addr, int(addr->ai_addrlen)) == 0) {
            break;
        }
        e = HD_GET_SOCKET_ERROR;
        msleep(0);
    }

    // retrieve bind info
    if (retries <= _maxRetries)
    {
        sockaddr_storage sa;
        socklen_t len = sizeof(sa);
        memset(&sa, 0, len);
        if (::getsockname(_sock, (sockaddr*)(&sa), &len) < 0) {
            e = HD_GET_SOCKET_ERROR;
            throw ESocketError("TcpSocket::bind() getsockname()", e);
        }

        if (sa.ss_family == AF_INET6) {
            const sockaddr_in6* sin = (const sockaddr_in6*)(&sa);
            _localPortBind = ntohs(sin->sin6_port);
        }
        else {
            const sockaddr_in* sin = (const sockaddr_in*)(&sa);
            _localPortBind = ntohs(sin->sin_port);
        }
    }

    // bind failed
    if (retries > _maxRetries) {
        char ebuf[1024];
        snprintf(ebuf, sizeof(ebuf), "TcpSocket::bind() BIND %d", _localPort);
        throw ESocketError(ebuf, e);
    }
}
void TcpSocket::close()
{
    if (_sock != HD_INVALID_SOCKET) {
        HD_CLOSESOCKET(_sock);
    }
    _sock = HD_INVALID_SOCKET;
    _cachedLocalAddr.ipv4.sin_family = AF_UNSPEC;
    _cachedPeerAddr.ipv4.sin_family = AF_UNSPEC;
}
u32 TcpSocket::available()
{
    unsigned long int bytes = 0;
    if (HD_IOCTRL(_sock, FIONREAD, &bytes) == -1) {
        int e = HD_GET_SOCKET_ERROR;
        throw ESocketError("TcpSocket::available()", e);
    }
    return bytes;
}

int TcpSocket::getSendTimeout()
{
    if (_sendTimeout > 0 || !isOpen()) {
        return _sendTimeout;
    }
    return getGenericTimeout(_sock, SO_SNDTIMEO);
}
int TcpSocket::getRecvTimeout()
{
    if (_recvTimeout > 0 || !isOpen()) {
        return _recvTimeout;
    }
    return getGenericTimeout(_sock, SO_RCVTIMEO);
}
int TcpSocket::getSendBufSize()
{
    if (_sendBufSize > 0 || !isOpen()) {
        return _sendBufSize;
    }
    return getGenericBufferSize(_sock, SO_SNDBUF);
}
int TcpSocket::getRecvBufSize()
{
    if (_recvBufSize > 0 || !isOpen()) {
        return _recvBufSize;
    }
    return getGenericBufferSize(_sock, SO_RCVBUF);
}
void TcpSocket::setRecvTimeout(int ms)
{
    setGenericTimeout(_sock, ms, SO_RCVTIMEO);
    _recvTimeout = ms;
}
void TcpSocket::setSendTimeout(int ms)
{
    setGenericTimeout(_sock, ms, SO_SNDTIMEO);
    _sendTimeout = ms;
}
void TcpSocket::setSendBufSize(int len)
{
    setGenericBufferSize(_sock, len, SO_SNDBUF);
    _sendBufSize = len;
}
void TcpSocket::setRecvBufSize(int len)
{
    setGenericBufferSize(_sock, len, SO_RCVBUF);
    _recvBufSize = len;
}
int TcpSocket::getGenericTimeout(HD_SOCKET sock, int optname)
{
    if (!isOpen()) {
        throw EInvalidOperation("TcpSocket::getGenericTimeout() socket isn't opened");
    }

#ifdef _WIN32
    DWORD to = 0;
    int len = sizeof(to);
#else
    struct timeval to = { 0, 0 };
    socklen_t len = sizeof(to);
#endif
    if (getsockopt(sock, SOL_SOCKET, optname, (char*)(&to), &len) == -1) {
        int e = HD_GET_SOCKET_ERROR;
        throw ESocketError("TcpSocket::getGenericTimeout()", e);
    }
#ifdef _WIN32
    return (int)to;
#else
    return (to.tv_sec * 1000 + to.tv_usec / 1000);
#endif
}
int TcpSocket::getGenericBufferSize(HD_SOCKET sock, int optname)
{
    if (!isOpen()) {
        throw EInvalidOperation("TcpSocket::getGenericBufferSize() socket isn't opened");
    }

#ifdef _WIN32
    int bufsize = 0, len = sizeof(bufsize);
#else
    int bufsize = 0;
    socklen_t len = sizeof(bufsize);
#endif

    if (getsockopt(sock, SOL_SOCKET, optname, (char*)(&bufsize), &len) == -1) {
        int e = HD_GET_SOCKET_ERROR;
        throw ESocketError("TcpSocket::getGenericBufferSize()", e);
    }
    return bufsize;
}
void TcpSocket::setGenericTimeout(HD_SOCKET sock, int timeout, int optname)
{
    if (timeout < 0) {
        throw EInvalidParam("TcpSocket::setGenericTimeout() invalid timeout value");
    }

    if (sock != HD_INVALID_SOCKET) {
#ifdef _WIN32
        DWORD to = (DWORD)(timeout);
#else
        struct timeval to = { (int)(timeout / 1000), (int)((timeout % 1000) * 1000) };
#endif
        if (setsockopt(sock, SOL_SOCKET, optname, (const char*)(&to), sizeof(to)) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("TcpSocket::setGenericTimeout()", e);
        }
    }
}
void TcpSocket::setGenericBufferSize(HD_SOCKET sock, int len, int optname)
{
    if (len <= 0) {
        throw EInvalidParam("TcpSocket::setGenericBufferSize() invalid len value");
    }

    if (sock != HD_INVALID_SOCKET) {
        if (setsockopt(sock, SOL_SOCKET, optname, (const char*)(&len), sizeof(len)) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("TcpSocket::setGenericBufferSize()", e);
        }
    }
}

void TcpSocket::setConnTimeout(int ms)
{
    _connTimeout = ms;
}
void TcpSocket::setLinger(bool on, int linger)
{
    _lingerOn = on;
    _lingerVal = linger;

    // l_onoff != 0 and l_linger = 0:
    // > Linux:   skip TIME_WAIT state when close socket
    // > Windows: skip TIME_WAIT state when close socket !! shutdown() should NOT be call !!
    if (_sock != HD_INVALID_SOCKET) {
#ifdef _WIN32
        struct linger l = { (_lingerOn ? 1 : 0), _lingerVal };
#else
        struct linger l = { (u_short)(_lingerOn ? 1 : 0), (u_short)(_lingerVal) };
#endif
        if (setsockopt(_sock, SOL_SOCKET, SO_LINGER, (const char*)(&l), sizeof(l)) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("TcpSocket::setLinger()", e);
        }
    }
}
void TcpSocket::setNoDelay(bool noDelay)
{
    _noDelay = noDelay;

    if (_sock != HD_INVALID_SOCKET) {
        // Set socket to NODELAY
        int v = _noDelay ? 1 : 0;
        if (setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)(&v), sizeof(v)) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("TcpSocket::setNoDelay()", e);
        }
    }
}
void TcpSocket::setKeepAlive(bool keepAlive)
{
    _keepAlive = keepAlive;

    if (_sock != HD_INVALID_SOCKET) {
        int val = _keepAlive;
        if (setsockopt(_sock, SOL_SOCKET, SO_KEEPALIVE, (const char*)(&val), sizeof(val)) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("TcpSocket::setKeepAlive()", e);
        }
    }
}
void TcpSocket::setMaxRecvRetries(int maxRecvRetries)
{
    _maxRetries = maxRecvRetries;
}

u32 TcpSocket::read(u8* buf, u32 len)
{
    if (_sock == HD_INVALID_SOCKET) {
        throw EInvalidOperation("TcpSocket::read() read on a non-open socket");
    }

    // HD_EAGAIN can be signalled both when a timeout has occurred and when
    // the system is out of resources (an awesome undocumented feature).
    // The following is an approximation of the time interval under which
    // HD_EAGAIN is taken to indicate an out of resources error.
    u32 eagainThresholdMicros = 0;
    if (_recvTimeout) {
        // if a readTimeout is specified along with a max number of recv retries, then
        // the threshold will ensure that the read timeout is not exceeded even in the
        // case of resource errors
        eagainThresholdMicros = (_recvTimeout * 1000) / ((_maxRetries > 0) ? _maxRetries : 2);
    }

    i32 retries = 0;

again:
    std::unique_ptr<ElapsedTimer> et;
    if (_recvTimeout > 0) {
        et.reset(new ElapsedTimer);
        et->start();
    }

#ifdef _WIN32
    // for block Winsock: minimum resolution of SO_RCVTIMEO is about 500ms
    if (true)
#else
    if (_interrupt)
#endif
    {
        HD_POLLFD fds[2];
        memset(fds, 0, sizeof(fds));
        fds[0].fd = _sock;
        fds[0].events = HD_POLLIN;
        if (_interrupt) {
            fds[1].fd = *(_interrupt.get());
            fds[1].events = HD_POLLIN;
        }

        int iRet = HD_POLL(fds, 2, (_recvTimeout == 0) ? -1 : _recvTimeout);
        if (iRet < 0)
        {
            // error cases
            int e = HD_GET_SOCKET_ERROR;
            if (e == HD_EINTR && (++retries <= _maxRetries))
            {
                goto again;
            }
            throw ESocketError("TcpSocket::read() HD_POLL()", e);
        }
        else if (iRet > 0)
        {
            // Check the interruptListener
            if (fds[1].revents & HD_POLLIN)
            {
                throw EInterrupt("TcpSocket::read() interrupted");
            }
        }
        else
        {
            throw ETimeout("TcpSocket::read() HD_POLL() timeout");
        }

        // falling through means there is something to recv and it cannot block
    }

    int iRet = recv(_sock, (char*)(buf), len, 0);
    if (iRet < 0)
    {
        int e = HD_GET_SOCKET_ERROR;
        if (e == HD_EAGAIN)
        {
            // if no timeout we can assume that resource exhaustion has occurred.
            if (_recvTimeout == 0) {
                throw ETimeout("TcpSocket::read() recv() unavailable resources");
            }
            // check if this is the lack of resources or timeout case
            if (et->uelapsed() < eagainThresholdMicros) {
                if (++retries <= _maxRetries) {
                    msleep(0);
                    goto again;
                }
                throw ETimeout(" TcpSocket::read() recv() unavailable resources");
            }
            // infer that timeout has been hit
            throw ETimeout("TcpSocket::read() recv() timeout");
        }

        // If interrupted, try again
        if (e == HD_EINTR && ++retries <= _maxRetries) {
            goto again;
        }

        if (e == HD_ETIMEDOUT) {
            throw ETimeout("TcpSocket::read() recv() timeout");
        }

        throw ESocketError("TcpSocket::read() recv()", e);
    }
    else if (iRet == 0) {
        throw ESocketError("TcpSocket::read() connection has been closed");
    }

    return iRet;
}
u32 TcpSocket::read(u8* buf, u32 len, u32 MinPacketLen)
{
    if (_sock == HD_INVALID_SOCKET) {
        throw EInvalidOperation("TcpSocket::read() read on a non-open socket");
    }
    int iRet = 0;
    // HD_EAGAIN can be signalled both when a timeout has occurred and when
    // the system is out of resources (an awesome undocumented feature).
    // The following is an approximation of the time interval under which
    // HD_EAGAIN is taken to indicate an out of resources error.
    u32 eagainThresholdMicros = 0;
    if (_recvTimeout) {
        // if a readTimeout is specified along with a max number of recv retries, then
        // the threshold will ensure that the read timeout is not exceeded even in the
        // case of resource errors
        eagainThresholdMicros = (_recvTimeout * 1000) / ((_maxRetries > 0) ? _maxRetries : 2);
    }

    i32 retries = 0;

again:
    std::unique_ptr<ElapsedTimer> et;
    if (_recvTimeout > 0) {
        et.reset(new ElapsedTimer);
        et->start();
    }

#ifdef _WIN32
    // for block Winsock: minimum resolution of SO_RCVTIMEO is about 500ms
    if (true)
#else
    if (_interrupt)
#endif
    {
        HD_POLLFD fds[2];
        memset(fds, 0, sizeof(fds));
        fds[0].fd = _sock;
        fds[0].events = HD_POLLIN;
        if (_interrupt) {
            fds[1].fd = *(_interrupt.get());
            fds[1].events = HD_POLLIN;
        }

        int iRet = HD_POLL(fds, 2, (_recvTimeout == 0) ? -1 : _recvTimeout);
        if (iRet < 0) {
            // error cases
            int e = HD_GET_SOCKET_ERROR;
            if (e == HD_EINTR && (++retries <= _maxRetries)) {
                goto again;
            }
            throw ESocketError("TcpSocket::read() HD_POLL()", e);
        }
        else if (iRet > 0) {
            // Check the interruptListener
            if (fds[1].revents & HD_POLLIN) {
                throw EInterrupt("TcpSocket::read() interrupted");
            }
        }
        else {
            throw ETimeout("TcpSocket::read() HD_POLL() timeout");
        }

        // falling through means there is something to recv and it cannot block
    }
    //当待接受包大于MIN_PACKET_LEN字节时，由于服务写包时进行了分包，因此读包时需要做对应操作
    if (len > MIN_PACKET_LEN)
    {
        //buf写入位置指示器
        int index = 0;
        //计算最后不足123个字节大小的剩余包长度
        u32 _packet_left_len = len % (MAX_PATCH_LEN - 5);
        //计算分包个数(循环分包次数)
        u32 _loop_times;
        (_packet_left_len == 0) ? (_loop_times = len / (MAX_PATCH_LEN - 5)) : (_loop_times = len / (MAX_PATCH_LEN - 5) + 1);
        std::shared_ptr<char> sp(new char[MAX_PATCH_LEN]);
        char* precv = sp.get();
        for (int i = 0; i < _loop_times; i++)
        {
            if (0 == i)
            {
                int offset = MAX_PATCH_LEN - MinPacketLen;
                //由于RPCReadPackage中读过包头，因此第一次读包需跳过MinPacketLen
                iRet = recv(_sock, precv, offset, 0);
                //判断包尾是否有分包标志位，从而判断这个包是否为长度大于128的包的分包
                if (iRet < 0)
                {
                    int e = HD_GET_SOCKET_ERROR;
                    if (e == HD_EAGAIN)
                    {
                        // if no timeout we can assume that resource exhaustion has occurred.
                        if (_recvTimeout == 0) {
                            throw ETimeout("TcpSocket::read() recv() unavailable resources");
                        }
                        // check if this is the lack of resources or timeout case
                        if (et->uelapsed() < eagainThresholdMicros) {
                            if (++retries <= _maxRetries) {
                                LOG_INFO("TcpSocket::read ： eagainThresholMicros : %d, retries : %d, maxRetries : %d", eagainThresholdMicros, retries, _maxRetries)
                                    msleep(20);
                                goto again;
                            }
                            throw ETimeout(" TcpSocket::read() recv() unavailable resources");
                        }
                        // infer that timeout has been hit
                        throw ETimeout("TcpSocket::read() recv() timeout");
                    }

                    // If interrupted, try again
                    if (e == HD_EINTR && ++retries <= _maxRetries) {
                        goto again;
                    }

                    if (e == HD_ETIMEDOUT) {
                        throw ETimeout("TcpSocket::read() recv() timeout");
                    }

                    throw ESocketError("TcpSocket::read() recv()", e);
                }
                else if (iRet == 0) {
                    throw ESocketError("TcpSocket::read() connection has been closed");
                }
                if ('$' == *(precv + offset - 5) && \
                    'M' == *(precv + offset - 4) && \
                    'M' == *(precv + offset - 3) && \
                    'S' == *(precv + offset - 2) && \
                    '$' == *(precv + offset - 1))
                {
                    //该包是分包的中间包
                    memcpy(buf, precv, offset - 5);
                    index += offset - 5;
                    LOG_INFO("TcpSocket::read : get depatched Packet Middle, len = %d, index size = %d", len, index)
                        LOG_INFO("TcpSocket::read signature:%d, %d, %d, %d, %d", *(precv + offset - 5), *(precv + offset - 4), *(precv + offset - 3), *(precv + offset - 2), *(precv + offset - 1))

                        //LOG_INFO("TcpSocket::read : get depatched Packet , len = %d, offset = %d, index = %d",len,offset,index);
                }
                else {
                    LOG_INFO("TcpSocket::read : get depatched Packet Wrong End")
                        LOG_INFO("TcpSocket::read signature:%d, %d, %d, %d, %d", *(precv + offset - 5), *(precv + offset - 4), *(precv + offset - 3), *(precv + offset - 2), *(precv + offset - 1))
                }
            }
            else if (i == _loop_times - 1)
            {
                if (_packet_left_len == 0)
                {
                    _packet_left_len = MAX_PATCH_LEN - 5;
                }
                iRet = recv(_sock, precv, _packet_left_len + 5, 0);
                if (iRet < 0)
                {
                    int e = HD_GET_SOCKET_ERROR;
                    if (e == HD_EAGAIN)
                    {
                        // if no timeout we can assume that resource exhaustion has occurred.
                        if (_recvTimeout == 0) {
                            throw ETimeout("TcpSocket::read() recv() unavailable resources");
                        }
                        // check if this is the lack of resources or timeout case
                        if (et->uelapsed() < eagainThresholdMicros) {
                            if (++retries <= _maxRetries) {
                                LOG_INFO("TcpSocket::read ： eagainThresholMicros : %d, retries : %d, maxRetries : %d", eagainThresholdMicros, retries, _maxRetries)
                                    msleep(20);
                                goto again;
                            }
                            throw ETimeout(" TcpSocket::read() recv() unavailable resources");
                        }
                        // infer that timeout has been hit
                        throw ETimeout("TcpSocket::read() recv() timeout");
                    }

                    // If interrupted, try again
                    if (e == HD_EINTR && ++retries <= _maxRetries) {
                        goto again;
                    }

                    if (e == HD_ETIMEDOUT) {
                        throw ETimeout("TcpSocket::read() recv() timeout");
                    }

                    throw ESocketError("TcpSocket::read() recv()", e);
                }
                else if (iRet == 0) {
                    throw ESocketError("TcpSocket::read() connection has been closed");
                }
                if ('$' == *(precv + _packet_left_len) && \
                    'E' == *(precv + _packet_left_len + 1) && \
                    'n' == *(precv + _packet_left_len + 2) && \
                    'd' == *(precv + _packet_left_len + 3) && \
                    '$' == *(precv + _packet_left_len + 4))
                {
                    //该包是分包结束包
                    memcpy(buf + index, precv, _packet_left_len);
                    index += _packet_left_len;
                    LOG_INFO("TcpSocket::read : get depatched Packet End, len = %d, index size = %d", len, index)
                        LOG_INFO("TcpSocket::read signature:%d, %d, %d, %d, %d", *(precv + _packet_left_len), *(precv + _packet_left_len + 1), *(precv + _packet_left_len + 2), *(precv + _packet_left_len + 3), *(precv + _packet_left_len + 4))

                        //LOG_INFO("TcpSocket::read : get depatched Packet End, len = %d, index = %d",len,index);
                }
                else {
                    LOG_INFO("TcpSocket::read : get depatched Packet Wrong End")
                        LOG_INFO("TcpSocket::read Error:%d, %d, %d, %d, %d", *(precv + _packet_left_len), *(precv + _packet_left_len + 1), *(precv + _packet_left_len + 2), *(precv + _packet_left_len + 3), *(precv + _packet_left_len + 4))
                }
            }
            else {
                iRet = recv(_sock, precv, MAX_PATCH_LEN, 0);
                //判断包尾是否有分包标志位，从而判断这个包是否为长度大于128的包的分包
                if (iRet < 0)
                {
                    int e = HD_GET_SOCKET_ERROR;
                    if (e == HD_EAGAIN)
                    {
                        // if no timeout we can assume that resource exhaustion has occurred.
                        if (_recvTimeout == 0) {
                            throw ETimeout("TcpSocket::read() recv() unavailable resources");
                        }
                        // check if this is the lack of resources or timeout case
                        if (et->uelapsed() < eagainThresholdMicros) {
                            if (++retries <= _maxRetries) {
                                LOG_INFO("TcpSocket::read ： eagainThresholMicros : %d, retries : %d, maxRetries : %d", eagainThresholdMicros, retries, _maxRetries)
                                    msleep(20);
                                goto again;
                            }
                            throw ETimeout(" TcpSocket::read() recv() unavailable resources");
                        }
                        // infer that timeout has been hit
                        throw ETimeout("TcpSocket::read() recv() timeout");
                    }

                    // If interrupted, try again
                    if (e == HD_EINTR && ++retries <= _maxRetries) {
                        goto again;
                    }

                    if (e == HD_ETIMEDOUT) {
                        throw ETimeout("TcpSocket::read() recv() timeout");
                    }

                    throw ESocketError("TcpSocket::read() recv()", e);
                }
                else if (iRet == 0) {
                    throw ESocketError("TcpSocket::read() connection has been closed");
                }
                if ('$' == *(precv + MAX_PATCH_LEN - 5) && \
                    'M' == *(precv + MAX_PATCH_LEN - 4) && \
                    'M' == *(precv + MAX_PATCH_LEN - 3) && \
                    'S' == *(precv + MAX_PATCH_LEN - 2) && \
                    '$' == *(precv + MAX_PATCH_LEN - 1))
                {
                    //该包是分包的中间包
                    memcpy(buf + index, precv, MAX_PATCH_LEN - 5);
                    LOG_INFO("TcpSocket::read signature:%d, %d, %d, %d, %d", *(precv + MAX_PATCH_LEN - 5), *(precv + MAX_PATCH_LEN - 4), *(precv + MAX_PATCH_LEN - 3), *(precv + MAX_PATCH_LEN - 2), *(precv + MAX_PATCH_LEN - 1))
                        index += (MAX_PATCH_LEN - 5);
                    LOG_INFO("TcpSocket::read : get depatched Packet Middle, len = %d, index size = %d", len, index)


                        //LOG_INFO("TcpSocket::read : get depatched Packet , len = %d, buf size = %d, index = %d",len,strlen((char*)buf),index);
                }
                else if ('$' == *(precv + MAX_PATCH_LEN - 5) && \
                    'E' == *(precv + MAX_PATCH_LEN - 4) && \
                    'n' == *(precv + MAX_PATCH_LEN - 3) && \
                    'd' == *(precv + MAX_PATCH_LEN - 2) && \
                    '$' == *(precv + MAX_PATCH_LEN - 1))
                {
                    //该包是分包结束包
                    memcpy(buf + index, precv, MAX_PATCH_LEN - 5);
                    index += MAX_PATCH_LEN - 5;
                    //LOG_INFO("TcpSocket::read : get depatched Packet End, len = %d, buf size = %d, index = %d",len,strlen((char*)buf),index);
                }
                else {
                    LOG_INFO("TcpSocket::read : get depatched Packet Wrong End")
                        LOG_INFO("TcpSocket::read signature:%d, %d, %d, %d, %d", *(precv + MAX_PATCH_LEN - 5), *(precv + MAX_PATCH_LEN - 4), *(precv + MAX_PATCH_LEN - 3), *(precv + MAX_PATCH_LEN - 2), *(precv + MAX_PATCH_LEN - 1))
                }
            }
        }
#if RPC_DEBUG_LOG
        LOG_INFO("TcpSocket::read : read index = %d", index)
#endif
            return index;

    }
    else {
        iRet = recv(_sock, (char*)(buf), len - MinPacketLen, 0);
        if (iRet < 0)
        {
            int e = HD_GET_SOCKET_ERROR;
            if (e == HD_EAGAIN)
            {
                // if no timeout we can assume that resource exhaustion has occurred.
                if (_recvTimeout == 0) {
                    throw ETimeout("TcpSocket::read() recv() unavailable resources");
                }
                // check if this is the lack of resources or timeout case
                if (et->uelapsed() < eagainThresholdMicros) {
                    if (++retries <= _maxRetries) {
                        LOG_INFO("TcpSocket::read ： eagainThresholMicros : %d, retries : %d, maxRetries : %d", eagainThresholdMicros, retries, _maxRetries)
                            msleep(20);
                        goto again;
                    }
                    throw ETimeout(" TcpSocket::read() recv() unavailable resources");
                }
                // infer that timeout has been hit
                throw ETimeout("TcpSocket::read() recv() timeout");
            }

            // If interrupted, try again
            if (e == HD_EINTR && ++retries <= _maxRetries) {
                goto again;
            }

            if (e == HD_ETIMEDOUT) {
                throw ETimeout("TcpSocket::read() recv() timeout");
            }

            throw ESocketError("TcpSocket::read() recv()", e);
        }
        else if (iRet == 0) {
            throw ESocketError("TcpSocket::read() connection has been closed");
        }
#if RPC_DEBUG_LOG
        LOG_INFO("TcpSocket::read : read index = %d", iRet)
#endif
            return iRet;
    }

}
u32 TcpSocket::writePartial(const u8* buf, u32 len)
{
    int iRet;
    if (_sock == HD_INVALID_SOCKET) {
        throw EInvalidOperation("TcpSocket::writePartial() write on a non-open socket");
    }

    int flags = 0;
#ifdef MSG_NOSIGNAL
    // Note the use of MSG_NOSIGNAL to suppress SIGPIPE errors, instead we
    // check for the HD_EPIPE return condition and close the socket in that case
    flags |= MSG_NOSIGNAL;
#endif
    if (len > MIN_PACKET_LEN && len < 1500)
    {
        //当待发包大小超过MIN_PACKET_LEN字节时，进行分包操作
        const u8* pos = buf;
        //计算最后不足MAX_PATCH_LEN-5个字节大小的剩余包长度
        u32 _packet_left_len = len % (MAX_PATCH_LEN - 5);
        //计算分包个数(循环分包次数)
        u32 _loop_times;
        (_packet_left_len == 0) ? (_loop_times = len / (MAX_PATCH_LEN - 5)) : (_loop_times = len / (MAX_PATCH_LEN - 5) + 1);
        std::shared_ptr<u8> sp(new u8[MAX_PATCH_LEN]);
        u8* patch_buf = sp.get();
        int _byte_sent = 0;
        for (int i = 0; i < _loop_times; i++)
        {
            if (_loop_times - 1 == i)
            {
                if (_packet_left_len == 0)
                {
                    _packet_left_len = MAX_PATCH_LEN - 5;
                }
                std::shared_ptr<u8> sp2(new u8[_packet_left_len + 5]);
                u8* final_buf = sp2.get();
                memcpy(final_buf, pos, _packet_left_len);
                //LOG_INFO("len = %d , _packet_left_len = %d, packet end = %d,%d",len,_packet_left_len,*(final_buf+_packet_left_len-1),*(final_buf+_packet_left_len))
                //在分包包尾加上简单字符校验
                memset(final_buf + _packet_left_len, '$', 1);
                memset(final_buf + _packet_left_len + 1, 'E', 1);
                memset(final_buf + _packet_left_len + 2, 'n', 1);
                memset(final_buf + _packet_left_len + 3, 'd', 1);
                memset(final_buf + _packet_left_len + 4, '$', 1);
                //LOG_INFO("TcpSocket::writePartial patch signature: %d, %d, %d, %d, %d",*(final_buf+_packet_left_len),*(final_buf+_packet_left_len+1),*(final_buf+_packet_left_len+2),*(final_buf+_packet_left_len+3),*(final_buf+_packet_left_len+4))
                //tcp发送
                iRet = send(_sock, (const char*)final_buf, _packet_left_len + 5, flags);
#if RPC_DEBUG_LOG & 0x0010
                LOG_INFO("TcpSocket::writePartial send patch packet, len = %d byte", _packet_left_len + 5)
#endif
                    if (iRet < 0) {
                        int e = HD_GET_SOCKET_ERROR;
                        if (e == HD_EWOULDBLOCK || e == HD_EAGAIN) {
                            return 0;
                        }
                        throw ESocketError("TcpSocket::writePartial() send()", e);
                    }

                // Fail on blocked send
                if (iRet == 0 && len != 0) {
                    throw ESocketError("TcpSocket::writePartial() send() 0 byte sent");
                }
                //当前低带宽电台发送缓存上限240 byte，发送设置100ms延时防止电台传输丢包
                msleep(100);
                _byte_sent += iRet;
            }
            else
            {
                memcpy(patch_buf, pos, MAX_PATCH_LEN - 5);
                pos += MAX_PATCH_LEN - 5;
                //在分包包尾加上简单字符校验
                memset(patch_buf + MAX_PATCH_LEN - 5, '$', 1);
                memset(patch_buf + MAX_PATCH_LEN - 4, 'M', 1);
                memset(patch_buf + MAX_PATCH_LEN - 3, 'M', 1);
                memset(patch_buf + MAX_PATCH_LEN - 2, 'S', 1);
                memset(patch_buf + MAX_PATCH_LEN - 1, '$', 1);
                //LOG_INFO("TcpSocket::writePartial patch signature: %d, %d, %d, %d, %d",*(patch_buf+123),*(patch_buf+123+1),*(patch_buf+123+2),*(patch_buf+123+3),*(patch_buf+123+4))
                //发送
                iRet = send(_sock, (const char*)patch_buf, MAX_PATCH_LEN, flags);
#if RPC_DEBUG_LOG & 0x0010
                LOG_INFO("TcpSocket::writePartial send patch packet, len = %d byte", MAX_PATCH_LEN - 5)
#endif
                    if (iRet < 0) {
                        int e = HD_GET_SOCKET_ERROR;
                        if (e == HD_EWOULDBLOCK || e == HD_EAGAIN) {
                            return 0;
                        }
                        throw ESocketError("TcpSocket::writePartial() send()", e);
                    }

                // Fail on blocked send
                if (iRet == 0 && len != 0) {
                    throw ESocketError("TcpSocket::writePartial() send() 0 byte sent");
                }
                _byte_sent += iRet;
                //当前低带宽电台发送缓存上限240 byte，发送设置100ms延时防止电台传输丢包
                msleep(100);
            }
        }
        return _byte_sent;
    }
    else {
        iRet = send(_sock, (const char*)buf, len, flags);

        if (iRet < 0) {
            int e = HD_GET_SOCKET_ERROR;
            if (e == HD_EWOULDBLOCK || e == HD_EAGAIN) {
                return 0;
            }
            throw ESocketError("TcpSocket::writePartial() send()", e);
        }

        // Fail on blocked send
        if (iRet == 0 && len != 0) {
            throw ESocketError("TcpSocket::writePartial() send() 0 byte sent");
        }
        //当前低带宽电台发送缓存上限240 byte，发送设置100ms延时防止电台传输丢包
        msleep(80);
        return iRet;
    }
}
void TcpSocket::write(const u8* buf, u32 len)
{
    u32 sent = 0;
    while (sent < len) {
        u32 b = writePartial(buf + sent, len - sent);
        //LOG_INFO("TcpSocket::write : len = %d , sent=%d , b=%d",len,sent,b)
        if (b == 0) {
            // This should only happen if the timeout set with SO_SNDTIMEO expired.
            // Raise an exception.
            throw ETimeout("TcpSocket::write() timeout");
        }
        sent += b;
    }
}


ECCS_END
