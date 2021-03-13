#ifndef MINI_MODUO_TCP_SERVER
#define MINI_MODUO_TCP_SERVER

#include "./decl.hh"
#include "./def.hh"
#include "./channel_callback.hh"
#include "./channel.hh"

#include <sys/epoll.h>

class TcpServer : public ChannelCallback {
  private:
    int m_epfd;
    int m_listenfd;
    epoll_event m_events[max_events];

    auto bind_and_listen() -> void;

    auto connect() -> void;

    auto echo(int sockfd) -> void;

  public:
    TcpServer();
    ~TcpServer() = default;

    virtual auto OnIn(int sockfd) -> void;

    auto run() -> void;
};

#endif