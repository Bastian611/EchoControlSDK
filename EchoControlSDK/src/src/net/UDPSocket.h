
#pragma once
#include "Sal_Socket.h"
#include "../utils/buffer.h"

ECCS_BEGIN


class UdpSocket
{
    NON_COPYABLE(UdpSocket);

public:
    UdpSocket(int localPort);                  // UdpServer
    UdpSocket(str remoteHost, int remotePort); // UdpClient
    ~UdpSocket();

    HD_SOCKET socket();

    bool filter() const;
    str remoteHost() const;
    int remotePort() const;
    void setRemoteAddr(str host, int port, bool filter);
    sockaddr* cachedRemoteAddress(socklen_t* len) const;

    str localHost() const;
    int localPort() const;  // bound port
    void setLocalAddr(str host, int port); // port = 0 means ANY
    sockaddr* cachedLocalAddress(socklen_t* len) const;

    bool isBroadcast() const;
    void setBroadcast(bool enable);
#if __linux
    // usually, binding local IP do the same thing, but UDP broadcast is special
    void bindIoDevice(const char* eth); // bind interface (root permision), e.g. eth="eth0"
#endif

    int getSendTimeout();
    int getRecvTimeout();
    int getSendBufSize();
    int getRecvBufSize();
    void setSendTimeout(int to /*ms*/);
    void setRecvTimeout(int to /*ms*/);
    void setSendBufSize(int len);
    void setRecvBufSize(int len);
    void setRetryLimit(int limit);

    bool isOpen() const;
    void open();
    void close();
    u32 available();  // last datagram length

    int read(u8* buf, u32 len);
    u32 writePartial(const u8* buf, u32 len);
    void write(const u8* buf, u32 len);

protected:
    void create(const addrinfo* res);
    void bind(const addrinfo* res);
    void setCachedLocalAddress(const sockaddr* addr, socklen_t len);
    void setCachedRemoteAddress(const sockaddr* addr, socklen_t len);

    int getGenericTimeout(HD_SOCKET sock, int optname);
    int getGenericBufferSize(HD_SOCKET sock, int optname);
    void setGenericTimeout(HD_SOCKET sock, int timeout/*ms*/, int optname);
    void setGenericBufferSize(HD_SOCKET sock, int len, int optname);

private:
    int   _remotePort;
    str   _remoteHost;
    int   _localPort;  // !! Linux: need root if < 1024; 0 means ANY !!
    int   _localPortBind;
    str   _localHost;
#if __linux
    str   _if;
#endif

    union {
        sockaddr_in  ipv4;
        sockaddr_in6 ipv6;
    } _cachedLocalAddr;
    union {
        sockaddr_in  ipv4;
        sockaddr_in6 ipv6;
    } _cachedRemoteAddr;

    bool  _filter;
    bool  _broadcast;
    int   _sndTimeout;
    int   _recvTimeout;
    int   _sndBufSize;
    int   _recvBufSize;
    int   _retryLimit;

    HD_SOCKET _sock;
};
typedef std::shared_ptr<UdpSocket> UdpSocket_Ptr;


ECCS_END
