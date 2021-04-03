#ifndef MINI_MUDUO_ECHO_SERVER
#define MINI_MUDUO_ECHO_SERVER

#include <string>

#include "./decl.hh"
#include "./http_request_parser.hh"
#include "./server.hh"
#include "./tcp_server_base.hh"

class EchoServer : public Server, public TcpServerBase {
  public:
    EchoServer(EventLoop* event_loop);
    ~EchoServer() = default;

    virtual auto on_connection(TcpConnection* tcp_connection) -> void;
    virtual auto on_message(TcpConnection* tcp_connection, Buffer& buffer) -> void;
    virtual auto on_write_done(TcpConnection* tcp_connection) -> void;

  private:
    auto echo(TcpConnection* tcp_connection, Buffer& buffer) -> void;

    EventLoop* mp_event_loop;
    RequestParser m_request_parser;
};

#endif