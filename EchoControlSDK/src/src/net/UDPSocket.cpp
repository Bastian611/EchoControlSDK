
#include "UDPSocket.h"
#include <string.h>
#include "../debug/exceptions.h"
#include "../debug/str_error.h"
#include "../time/elapsed_timer.h"
#include "../time/time_utils.h"
#include "../utils/utils.h"

ECCS_BEGIN


UdpSocket::UdpSocket(int localPort) : _localPort(localPort)
{
    _localPortBind = _localPort;
    _remotePort = HD_INVALID_PORT;

    _filter = false;
    _sndTimeout = 0;
    _recvTimeout = 0;
    _sndBufSize = 0;
    _recvBufSize = 0;
    _retryLimit = 5;

    _sock = HD_INVALID_SOCKET;

    memset(&_cachedLocalAddr, 0, sizeof(_cachedLocalAddr));
    memset(&_cachedRemoteAddr, 0, sizeof(_cachedRemoteAddr));
    _cachedLocalAddr.ipv4.sin_family = AF_UNSPEC;
    _cachedRemoteAddr.ipv4.sin_family = AF_UNSPEC;
}
UdpSocket::UdpSocket(str remoteHost, int remotePort)
    : _remotePort(remotePort), _remoteHost(remoteHost)
{
    _localPort = HD_INVALID_PORT;
    _localPortBind = _localPort;

    _filter = false;
    _sndTimeout = 0;
    _recvTimeout = 0;
    _sndBufSize = 0;
    _recvBufSize = 0;
    _retryLimit = 5;

    _sock = HD_INVALID_SOCKET;

    memset(&_cachedLocalAddr, 0, sizeof(_cachedLocalAddr));
    memset(&_cachedRemoteAddr, 0, sizeof(_cachedRemoteAddr));
    _cachedLocalAddr.ipv4.sin_family = AF_UNSPEC;
    _cachedRemoteAddr.ipv4.sin_family = AF_UNSPEC;
}
UdpSocket::~UdpSocket()
{
    close();
}
HD_SOCKET UdpSocket::socket()
{
    return _sock;
}

str UdpSocket::remoteHost() const
{
    return _remoteHost;
}
int UdpSocket::remotePort() const
{
    return _remotePort;
}
void UdpSocket::setRemoteAddr(str host, int port, bool filter)
{
    if (isOpen()) {
        throw EInvalidOperation("UdpSocket::setRemoteAddr() should be call before open()");
    }
    _filter = filter;
    _remoteHost = host;
    _remotePort = port;
}

str UdpSocket::localHost() const
{
    return _localHost;
}
int UdpSocket::localPort() const
{
    return _localPortBind;
}
void UdpSocket::setLocalAddr(str host, int port)
{
    if (isOpen()) {
        throw EInvalidOperation("UdpSocket::setLocalAddr() should be call before open()");
    }
    _localHost = host;
    _localPort = port;
}

void UdpSocket::setCachedLocalAddress(const sockaddr* addr, socklen_t len)
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
void UdpSocket::setCachedRemoteAddress(const sockaddr* addr, socklen_t len)
{
    switch (addr->sa_family)
    {
    case AF_INET:
        if (len == sizeof(sockaddr_in)) {
            memcpy((void*)&_cachedRemoteAddr.ipv4, (void*)addr, len);
        }
        break;
    case AF_INET6:
        if (len == sizeof(sockaddr_in6)) {
            memcpy((void*)&_cachedRemoteAddr.ipv6, (void*)addr, len);
        }
        break;
    default:
        break;
    }
}
sockaddr* UdpSocket::cachedLocalAddress(socklen_t* len) const
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
sockaddr* UdpSocket::cachedRemoteAddress(socklen_t* len) const
{
    switch (_cachedRemoteAddr.ipv4.sin_family)
    {
    case AF_INET:
        if (len) {
            *len = sizeof(sockaddr_in);
        }
        return (sockaddr*)&_cachedRemoteAddr.ipv4;
    case AF_INET6:
        if (len) {
            *len = sizeof(sockaddr_in6);
        }
        return (sockaddr*)&_cachedRemoteAddr.ipv6;
    default:
        return NULL;
    }
}

bool UdpSocket::isBroadcast() const
{
    return _broadcast;
}
void UdpSocket::setBroadcast(bool enable)
{
    _broadcast = enable;

    if (_sock != HD_INVALID_SOCKET) {
#ifdef _WIN32
        BOOL val = _broadcast ? TRUE : FALSE;
#else
        int val = _broadcast ? 1 : 0;
#endif
        if (setsockopt(_sock, SOL_SOCKET, SO_BROADCAST, (const char*)(&val), sizeof(val)) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("UdpSocket::setBroadcast()", e);
        }
    }
}
#ifdef __linux
void UdpSocket::bindIoDevice(const char* eth)
{
    _if = eth;

    if (_sock != HD_INVALID_SOCKET && !_if.empty()) {
        if (setsockopt(_sock, SOL_SOCKET, SO_BINDTODEVICE, _if.c_str(), _if.length()) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("UdpSocket::bindIoDevice()", e);
        }
    }
}
#endif

int UdpSocket::getSendTimeout()
{
    if (_sndTimeout > 0 || !isOpen()) {
        return _sndTimeout;
    }
    return getGenericTimeout(_sock, SO_SNDTIMEO);
}
int UdpSocket::getRecvTimeout()
{
    if (_recvTimeout > 0 || !isOpen()) {
        return _recvTimeout;
    }
    return getGenericTimeout(_sock, SO_RCVTIMEO);
}
int UdpSocket::getSendBufSize()
{
    if (_sndBufSize > 0 || !isOpen()) {
        return _sndBufSize;
    }
    return getGenericBufferSize(_sock, SO_SNDBUF);
}
int UdpSocket::getRecvBufSize()
{
    if (_recvBufSize > 0 || !isOpen()) {
        return _recvBufSize;
    }
    return getGenericBufferSize(_sock, SO_RCVBUF);
}
int UdpSocket::getGenericTimeout(HD_SOCKET sock, int optname)
{
    if (!isOpen()) {
        throw EInvalidOperation("UdpSocket::getGenericTimeout() socket isn't opened");
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
        throw ESocketError("UdpSocket::getGenericTimeout()", e);
    }
#ifdef _WIN32
    return (int)to;
#else
    return (to.tv_sec * 1000 + to.tv_usec / 1000);
#endif
}
int UdpSocket::getGenericBufferSize(HD_SOCKET sock, int optname)
{
    if (!isOpen()) {
        throw EInvalidOperation("UdpSocket::getGenericBufferSize() socket isn't opened");
    }

#ifdef _WIN32
    int bufsize = 0, len = sizeof(bufsize);
#else
    int bufsize = 0;
    socklen_t len = sizeof(bufsize);
#endif

    if (getsockopt(sock, SOL_SOCKET, optname, (char*)(&bufsize), &len) == -1) {
        int e = HD_GET_SOCKET_ERROR;
        throw ESocketError("UdpSocket::getGenericBufferSize()", e);
    }
    return bufsize;
}

void UdpSocket::setSendTimeout(int to /*ms*/)
{
    setGenericTimeout(_sock, to, SO_SNDTIMEO);
    _sndTimeout = to;
}
void UdpSocket::setRecvTimeout(int to /*ms*/)
{
    setGenericTimeout(_sock, to, SO_RCVTIMEO);
    _recvTimeout = to;
}
void UdpSocket::setSendBufSize(int len)
{
    setGenericBufferSize(_sock, len, SO_SNDBUF);
    _sndBufSize = len;
}
void UdpSocket::setRecvBufSize(int len)
{
    setGenericBufferSize(_sock, len, SO_RCVBUF);
    _recvBufSize = len;
}
void UdpSocket::setRetryLimit(int limit)
{
    _retryLimit = limit;
}
void UdpSocket::setGenericTimeout(HD_SOCKET sock, int timeout/*ms*/, int optname)
{
    if (timeout < 0) {
        throw EInvalidParam("UdpSocket::setGenericTimeout() invalid timeout value");
    }

    if (sock != HD_INVALID_SOCKET) {
#ifdef _WIN32
        DWORD to = (DWORD)(timeout);
#else
        struct timeval to = { (int)(timeout / 1000), (int)((timeout % 1000) * 1000) };
#endif
        if (setsockopt(sock, SOL_SOCKET, optname, (const char*)(&to), sizeof(to)) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("UdpSocket::setGenericTimeout()", e);
        }
    }
}
void UdpSocket::setGenericBufferSize(HD_SOCKET sock, int len, int optname)
{
    if (len <= 0) {
        throw EInvalidParam("UdpSocket::setGenericBufferSize() invalid len value");
    }

    if (sock != HD_INVALID_SOCKET) {
        if (setsockopt(sock, SOL_SOCKET, optname, (const char*)(&len), sizeof(len)) == -1) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("UdpSocket::setGenericBufferSize()", e);
        }
    }
}

bool UdpSocket::isOpen() const
{
    return (_sock != HD_INVALID_SOCKET);
}
void UdpSocket::open()
{
    if (isOpen()) {
        return;
    }
    if (_filter && (!IsValidPort(_remotePort) || _remoteHost.empty())) {
        throw EInvalidParam("UdpSocket::open() invalid remote address: %s:%d", _remoteHost.c_str(), _remotePort);
    }
    SocketStartup::startup();

    // If remote address is specified, create socket
    if (IsValidPort(_remotePort) && !_remoteHost.empty())
    {
        AddrInfoWrapper remoteAi(_remoteHost, _remotePort, SOCK_DGRAM);
        int remoteRet = remoteAi.init();
        if (remoteRet) {
            close();
            str eStr = "UdpSocket::open() getaddrinfo() remote address: " + str(HD_GAI_STRERROR(remoteRet));
            throw ESocketError(eStr.c_str());
        }

        const addrinfo* res = remoteAi.res();
        for (; res; res = res->ai_next) {
            try { create(res); break; }
            catch (...) {
                close();
                if (!(res->ai_next)) throw;
            }
        }

        setCachedRemoteAddress(res->ai_addr, (socklen_t)res->ai_addrlen);
    }

    if (IsValidPort(_localPort))
    {
        AddrInfoWrapper localAi(_localHost, _localPort, SOCK_DGRAM);
        int localRet = localAi.init();
        if (localRet) {
            close();
            str eStr = "UdpSocket::open() getaddrinfo() local address: " + str(HD_GAI_STRERROR(localRet));
            throw ESocketError(eStr.c_str());
        }

        const addrinfo* res = localAi.res();
        if (_sock != HD_INVALID_SOCKET)
        {
            for (; res; res = res->ai_next)
            {
                try { bind(res); break; }
                catch (...) {
                    if (!(res->ai_next)) {
                        close();
                        throw;
                    }
                }
            }
        }
        else
        {
            for (; res; res = res->ai_next)
            {
                try {
                    create(res);
                    bind(res);
                    break;
                }
                catch (...) {
                    close();
                    if (!(res->ai_next)) throw;
                }
            }
        }
    }

    if (_sock == HD_INVALID_SOCKET) {
        close();
        throw EInvalidParam("UdpSocket::open() invalid paramter for open");
    }
}
void UdpSocket::create(const addrinfo* res)
{
    if (_sock == HD_INVALID_SOCKET) {
        _sock = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (_sock == HD_INVALID_SOCKET) {
            int e = HD_GET_SOCKET_ERROR;
            throw ESocketError("UdpSocket::create() socket()", e);
        }
    }

    try
    {
        if (_sndTimeout > 0) {
            setSendTimeout(_sndTimeout);
        }
        if (_recvTimeout > 0) {
            setRecvTimeout(_recvTimeout);
        }
        if (_sndBufSize > 0) {
            setSendBufSize(_sndBufSize);
        }
        if (_recvBufSize > 0) {
            setRecvBufSize(_recvBufSize);
        }
        if (_broadcast) {
            setBroadcast(_broadcast);
        }
#ifdef __linux
        if (!_if.empty()) {
            bindIoDevice(_if.c_str());
        }
#endif
    }
    catch (...) {
        throw;
    }

#ifdef IPV6_V6ONLY
    if (res->ai_addr && res->ai_family == AF_INET6) {
        int zero = 0;
        setsockopt(_sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)(&zero), sizeof(zero));
    }
#endif

#ifdef SO_NOSIGPIPE
    {
        int one = 1;
        setsockopt(socket_, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof(one));
    }
#endif
}
void UdpSocket::bind(const addrinfo* res)
{
    if (_sock == HD_INVALID_SOCKET) {
        throw EInvalidParam("UdpSocket::bind() invalid socket");
    }

    // prepare the port information
    // we may want to try to bind more than once, since HD_NO_SOCKET_CACHING doesn't
    // always seem to work. The client can configure the retry variables.
    int retries = 0, e = 0;
    while (++retries <= _retryLimit) {
        if (::bind(_sock, res->ai_addr, int(res->ai_addrlen)) == 0) {
            break;
        }
        e = HD_GET_SOCKET_ERROR;
        msleep(0);
    }

    // retrieve bind info
    if (retries <= _retryLimit)
    {
        sockaddr_storage sa;
        socklen_t len = sizeof(sa);
        memset(&sa, 0, len);
        if (::getsockname(_sock, (sockaddr*)(&sa), &len) < 0) {
            e = HD_GET_SOCKET_ERROR;
            throw ESocketError("UdpSocket::open() getsockname()", e);
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

    if (retries > _retryLimit) {
        char ebuf[1024];
        snprintf(ebuf, sizeof(ebuf), "UdpSocket::open() BIND %d", _localPort);
        throw ESocketError(ebuf, e);
    }

    setCachedLocalAddress(res->ai_addr, (socklen_t)res->ai_addrlen);
}
void UdpSocket::close()
{
    if (_sock != HD_INVALID_SOCKET) {
        shutdown(_sock, HD_SHUT_RDWR);
        HD_CLOSESOCKET(_sock);
    }
    _cachedLocalAddr.ipv4.sin_family = AF_UNSPEC;
    _cachedRemoteAddr.ipv4.sin_family = AF_UNSPEC;
    _sock = HD_INVALID_SOCKET;
}
u32 UdpSocket::available()
{
    unsigned long int bytes = 0;
    if (HD_IOCTRL(_sock, FIONREAD, &bytes) == -1) {
        int e = HD_GET_SOCKET_ERROR;
        throw ESocketError("UdpSocket::available()", e);
    }
    return bytes;
}

int UdpSocket::read(u8* buf, u32 len)
{
    if (_sock == HD_INVALID_SOCKET)
    {
        throw EInvalidOperation("UdpSocket::read() read on a non-open socket");
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
        eagainThresholdMicros = (_recvTimeout * 1000) / ((_retryLimit > 0) ? _retryLimit : 2);
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
    {
        HD_POLLFD fds[1];
        memset(fds, 0, sizeof(fds));
        fds[0].fd = _sock;
        fds[0].events = HD_POLLIN;

        int iRet = HD_POLL(fds, 1, (_recvTimeout == 0) ? -1 : _recvTimeout);
        if (iRet < 0) {
            // error cases
            int e = HD_GET_SOCKET_ERROR;
            if (e == HD_EINTR && (++retries <= _retryLimit)) {
                goto again;
            }
            throw ESocketError("UdpSocket::peek() HD_POLL()", e);
        }
        else if (iRet == 0) {
            // timeout
            throw ETimeout("UdpSocket::peek() HD_POLL() timeout");
        }

        // There must be data or a disconnection, fall through to the recvfrom
    }
#endif

    sockaddr_storage ss;
    socklen_t addrLen = sizeof(ss);
    memset(&ss, 0, addrLen);
    int iRet = recvfrom(_sock, (char*)(buf), len, 0, (sockaddr*)&ss, &addrLen);

    if (iRet < 0)
    {
        int e = HD_GET_SOCKET_ERROR;
        if (e == HD_EAGAIN)
        {
            // if no timeout we can assume that resource exhaustion has occurred.
            if (_recvTimeout == 0) {
                throw ETimeout("UdpSocket::read() recv() unavailable resources");
            }
            // check if this is the lack of resources or timeout case
            if (et->uelapsed() < eagainThresholdMicros) {
                if (++retries <= _retryLimit) {
                    msleep(0);
                    goto again;
                }
                throw ETimeout("UdpSocket::read() recv() unavailable resources");
            }
            // infer that timeout has been hit
            throw ETimeout("UdpSocket::read() recv() timeout");
        }

        // If interrupted, try again
        if (e == HD_EINTR && ++retries <= _retryLimit) {
            goto again;
        }

        if (e == HD_ETIMEDOUT) {
            throw ETimeout("UdpSocket::read() recv() timeout");
        }

        throw ESocketError("UdpSocket::read() recv()", e);
    }
    else if (iRet == 0) {
        throw ESocketError("UdpSocket::read() connection has been closed");
    }

    // filter
    if (_filter) {
        auto* cachedAddr = cachedRemoteAddress(NULL);
        if (cachedAddr && !Equal(*(sockaddr*)&ss, *cachedAddr)) {
            return 0;  // drop data recvfrom()
        }
    }
    else {
        setCachedRemoteAddress((sockaddr*)&ss, addrLen);
    }

    return iRet;
}
u32 UdpSocket::writePartial(const u8* buf, u32 len)
{
    if (_sock == HD_INVALID_SOCKET) {
        throw EInvalidOperation("UdpSocket::writePartial() write on a non-open socket");
    }

    // get remote address
    socklen_t addrLen = 0;
    auto* addr = cachedRemoteAddress(&addrLen);
    if (!addr) {
        throw EInvalidOperation("UdpSocket::writePartial() no valid remote address");
    }

    int flags = 0;
#ifdef MSG_NOSIGNAL
    // Note the use of MSG_NOSIGNAL to suppress SIGPIPE errors, instead we
    // check for the HD_EPIPE return condition and close the socket in that case
    flags |= MSG_NOSIGNAL;
#endif

    int iRet = sendto(_sock, (const char*)buf, len, flags, addr, addrLen);

    if (iRet < 0) {
        int e = HD_GET_SOCKET_ERROR;
        if (e == HD_EWOULDBLOCK || e == HD_EAGAIN) {
            return 0;
        }
        throw ESocketError("UdpSocket::writePartial() send()", e);
    }

    // Fail on blocked send
    if (iRet == 0 && len != 0) {
        throw ESocketError("UdpSocket::writePartial() send() 0 byte sent");
    }

    return iRet;
}
void UdpSocket::write(const u8* buf, u32 len)
{
    u32 sent = 0;
    while (sent < len) {
        u32 b = writePartial(buf + sent, len - sent);
        if (b == 0) {
            // This should only happen if the timeout set with SO_SNDTIMEO expired.
            // Raise an exception.
            throw ETimeout("UdpSocket::write() timeout");
        }
        sent += b;
    }
}


ECCS_END
