#ifndef MINI_MODUO_TCP_CONNECTION
#define MINI_MODUO_TCP_CONNECTION

#include "./decl.hh"
#include "./channel_callback.hh"

class TcpConnectionHandler : public ChannelCallback {
  public:
    TcpConnectionHandler(int epfd, int connfd);
    ~TcpConnectionHandler() = default;

    virtual auto on_in(int sockfd) -> void;

  private:
    int m_epfd;
    int m_connfd;
    Channel* mp_channel;
};

#endif