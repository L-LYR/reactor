#ifndef MINI_MODUO_ACCEPTOR
#define MINI_MODUO_ACCEPTOR

#include "./decl.hh"
#include "./def.hh"
#include "./callback.hh"

class Acceptor : public ChannelCallback {
  public:
    Acceptor(int epfd);
    ~Acceptor() = default;

    auto run() -> void;
    auto set_callback(AcceptorCallback* p_acceptor_callback) -> void;

    virtual auto on_in(int sockfd) -> void;

  private:
    auto bind_and_listen() -> void;

    int m_epfd;
    int m_listenfd;
    Channel* mp_channel;
    AcceptorCallback* mp_acceptor_callback;
};

#endif