#ifndef MINI_MUDUO_TCP_CONNECTION
#define MINI_MUDUO_TCP_CONNECTION

#include "./def.hh"
#include "./decl.hh"
#include "./callback.hh"
#include "./http_request_parser.hh"

#include <string>

class TcpConnection : public ChannelCallback {
  public:
    TcpConnection(EventLoop* event_loop, int connfd);
    ~TcpConnection() = default;

    virtual auto on_in(int sockfd) -> void;

    auto regester_in(Server* server) -> void;
    auto establish() -> void;

    auto write(const std::string& data) -> void;

  private:
    int m_connfd;
    Channel* mp_channel;
    EventLoop* mp_event_loop;
    Server* mp_server;

    char m_read_buffer[max_lines + 1];
    int m_read_length;
};

#endif