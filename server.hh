#ifndef MINI_MUDUO_SERVER
#define MINI_MUDUO_SERVER

#include "./tcp_connection.hh"
#include "./buffer.hh"

class Server {
  public:
    virtual auto on_connection(TcpConnection* tcp_connection) -> void = 0;
    virtual auto on_message(TcpConnection* tcp_connection, Buffer& buffer) -> void = 0;
    virtual auto on_write_done(TcpConnection* tcp_connection) -> void = 0;
};

#endif