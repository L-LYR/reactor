#ifndef MINI_MUDUO_ECHO_SERVER
#define MINI_MUDUO_ECHO_SERVER

#include "./decl.hh"
#include "./server.hh"
#include "./tcp_server_base.hh"

#include <string>

class EchoServer : public Server {
  public:
    EchoServer(EventLoop* event_loop);
    ~EchoServer() = default;

    virtual auto on_connection(TcpConnection* tcp_connection) -> void;
    virtual auto on_message(TcpConnection* tcp_connection, const std::string& data) -> void;

    auto run() -> void;


  private:
    auto echo(TcpConnection* tcp_connection, const std::string& data) -> void;
    
    EventLoop* mp_event_loop;
    TcpServerBase m_tcp_server_base;
    RequestParser m_request_parser;
};

#endif