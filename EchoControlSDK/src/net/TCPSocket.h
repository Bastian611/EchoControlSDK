
#pragma once
#include "Sal_Socket.h"

ECCS_BEGIN


//------------------------------------------------------
// TcpSocket:
// 1. Used as TCP client
//    A. No timeout is set by default
//    B. Skip TIME_WAIT state by default
//       > Skip TIME_WAIT by setting linger{1, 0}
//         On Windows, require no call of shutdown()
//       > This may cause data in buffer lost
//       > Skip TIME_WAIT is not a good idea
// 2. Adopting socket accepted by TCP server
//    A. Timeout may be already set by accept() side
//    B. The socket that TcpServer::accept() returned
//       will not Skip TIME_WAIT
//------------------------------------------------------

class TcpSocket
{
    NON_COPYABLE(TcpSocket);

public:
    //每个分包的长度
    static const int MAX_PATCH_LEN = 400;
    //需要进行分包操作的最小包大小
    static const int MIN_PACKET_LEN = 512;
    TcpSocket(str host, int port);
    TcpSocket(HD_SOCKET sock, std::shared_ptr<HD_SOCKET> interrupt = NULL);
    ~TcpSocket();

    str host() const;
    int port() const;
    str socketInfo();
    HD_SOCKET socket();

    str localHost() const;
    int localPort() const;
    void setLocalAddr(str host, int port);
    void setCachedLocalAddress(const sockaddr* addr, socklen_t len);
    sockaddr* cachedLocalAddress(socklen_t* len) const;

    str peerHost();
    str peerAddress();
    int peerPort();
    str origin();
    sockaddr* cachedPeerAddress(socklen_t* len) const;
    void setCachedPeerAddress(const sockaddr* addr, socklen_t len);

    bool isOpen() const;
    void open();
    void close();
    u32 available();

    int getSendTimeout();
    int getRecvTimeout();
    int getSendBufSize();
    int getRecvBufSize();
    void setRecvTimeout(int ms);
    void setSendTimeout(int ms);
    void setSendBufSize(int len);
    void setRecvBufSize(int len);

    void setConnTimeout(int ms);
    void setLinger(bool on, int linger);
    void setNoDelay(bool noDelay);
    void setKeepAlive(bool keepAlive);
    void setMaxRecvRetries(int maxRecvRetries);

    u32 read(u8* buf, u32 len);
    /**
     * @brief read：根据偏移量读取分包传输的rpc包
     * @param buf:读缓冲
     * @param len:缓冲区长度
     * @param MinPacketLen：缓冲区偏移量，含义为RPC包的包头长度
     * @return 读取的字节数
     */
    u32 read(u8* buf, u32 len, u32 MinPacketLen);
    u32 writePartial(const u8* buf, u32 len);
    void write(const u8* buf, u32 len);

protected:
    void open(const addrinfo* addr);
    void bind(const addrinfo* res);
    void setGenericTimeout(HD_SOCKET sock, int timeout/*ms*/, int optname);

    int getGenericTimeout(HD_SOCKET sock, int optname);
    int getGenericBufferSize(HD_SOCKET sock, int optname);
    void setGenericBufferSize(HD_SOCKET sock, int len, int optname);

private:
    str   _host;
    int   _port;
    str   _localHost;
    int   _localPort;  // !! Linux: need root if < 1024 !!
    int   _localPortBind;

    str   _peerHost;
    str   _peerAddr;
    int   _peerPort;

    int   _connTimeout;  // ms
    int   _recvTimeout;
    int   _sendTimeout;
    int   _sendBufSize;
    int   _recvBufSize;
    int   _maxRetries;
    bool  _keepAlive;
    bool  _noDelay;

    int   _lingerVal;
    bool  _lingerOn;

    union {
        sockaddr_in  ipv4;
        sockaddr_in6 ipv6;
    } _cachedLocalAddr;
    union {
        sockaddr_in  ipv4;
        sockaddr_in6 ipv6;
    } _cachedPeerAddr;

    HD_SOCKET _sock;
    std::shared_ptr<HD_SOCKET> _interrupt;
};
typedef std::shared_ptr<TcpSocket> TcpSocket_Ptr;


ECCS_END
