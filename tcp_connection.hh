#ifndef MINI_MUDUO_TCP_CONNECTION
#define MINI_MUDUO_TCP_CONNECTION

#include <string>

#include "./buffer.hh"
#include "./callback.hh"
#include "./decl.hh"
#include "./def.hh"
#include "./runnable.hh"

class TcpConnection : public ChannelCallback, public Runnable {
  public:
    TcpConnection(EventLoop* event_loop, int connfd);
    ~TcpConnection() = default;

    virtual auto handle_read() -> void;
    virtual auto handle_write() -> void;
    virtual auto run() -> void;

    auto regester_in(Server* server) -> void;
    auto establish() -> void;

    auto send(const char* data, size_t length) -> void;

  private:
    Channel* mp_channel;
    EventLoop* mp_event_loop;
    Server* mp_server;
    Buffer m_in_buffer;
    Buffer m_out_buffer;
};

#endif