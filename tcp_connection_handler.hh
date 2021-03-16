#ifndef MINI_MUDUO_TCP_CONNECTION
#define MINI_MUDUO_TCP_CONNECTION

#include "./decl.hh"
#include "./callback.hh"
#include "./http_request_parser.hh"

class TcpConnectionHandler : public ChannelCallback {
  public:
    TcpConnectionHandler(EventLoop* event_loop, int connfd);
    ~TcpConnectionHandler() = default;

    virtual auto on_in(int sockfd) -> void;

  private:
    int m_connfd;
    Channel* mp_channel;
    EventLoop* mp_event_loop;
};

#endif