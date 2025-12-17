
#pragma once
#include "tcpsocket.h"

ECCS_BEGIN


class TcpServer
{
    NON_COPYABLE(TcpServer);

public:
    TcpServer(int port);
    TcpServer(str addr, int port);
    ~TcpServer();

    int port() const { return _portBind; }
    str address() const { return _addr; }
    bool isListening() const { return _listening; }
    sockaddr* cachedLocalAddress(socklen_t* len) const;

    // for client socket
    void setSendTimeout(int to);
    void setRecvTimeout(int to);
    void setKeepAlive(bool keepAlive);

    // for server socket, set before listening
    void setAcceptTimeout(int to);  // 0: non-blocking accept
    void setAcceptBacklog(int backlog);
    void setRetryLimit(int limit);
    void setRetryDelay(int delay);
    void setSendBuffer(int len);
    void setRecvBuffer(int len);

    typedef std::function<void(HD_SOCKET)> callback;
    void setListenCallback(const callback& cb) { _listenCallback = cb; }  // on listen socket
    void setAcceptCallback(const callback& cb) { _acceptCallback = cb; }  // on accepted socket

    // When enabled (the default), new children TSockets will be constructed so
    // they can be interrupted by TServerTransport::interruptChildren().
    // This is more expensive in terms of system calls (poll + recv) however
    // ensures a connected client cannot interfere with TServer::stop().
    //
    // When disabled, TSocket children do not incur an additional poll() call.
    // Server-side reads are more efficient, however a client can interfere with
    // the server's ability to shutdown properly by staying connected.
    void setInterruptableChildren(bool enable);

    void listen();
    void interrupt();
    void interruptChildren();
    void close();

    // setAcceptTimeout(0): non-blocking accept, and return null pointer
    // if no accepted client.
    TcpSocket_Ptr accept();

protected:
    void notify(HD_SOCKET sock);
    void setCachedLocalAddress(const sockaddr* addr, socklen_t len);

private:
    int  _port;
    int  _portBind;
    str  _addr;
    int  _acceptBacklog;
    int  _sendTimeout;
    int  _recvTimeout;
    int  _accTimeout;
    int  _retryLimit;
    int  _retryDelay;
    int  _sendBufSize;
    int  _recvBufSize;
    bool _keepAlive;
    bool _listening;
    bool _interruptableChildren;

    union {
        sockaddr_in  ipv4;
        sockaddr_in6 ipv6;
    } _cachedLocalAddr;

    HD_SOCKET  _serverSock;
    HD_SOCKET  _interruptWriter;       // is notified on interrupt()
    HD_SOCKET  _interruptReader;       // is used in select/poll with _serverSock for interruptability
    HD_SOCKET  _interrupChildWriter;   // is notified on interruptChildren()
    std::shared_ptr<HD_SOCKET> _interruptChildReader; // if _interruptableChildren, this is shared with child TSockets

    callback _listenCallback;
    callback _acceptCallback;
};
typedef std::shared_ptr<TcpServer> TcpServer_Ptr;


ECCS_END
