#ifndef MINI_MUDUO_ACCEPTOR
#define MINI_MUDUO_ACCEPTOR

#include "./decl.hh"
#include "./def.hh"
#include "./callback.hh"

class Acceptor : public ChannelCallback {
  public:
    Acceptor(EventLoop* event_loop);
    ~Acceptor() = default;

    auto run() -> void;
    auto set_callback(AcceptorCallback* p_acceptor_callback) -> void;

    virtual auto on_in(int sockfd) -> void;

  private:
    auto bind_and_listen() -> void;

    EventLoop* mp_event_loop;

    int m_listenfd;
    Channel* mp_channel;
    AcceptorCallback* mp_acceptor_callback;
};

#endif