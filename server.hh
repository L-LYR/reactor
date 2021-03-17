#ifndef MINI_MUDUO_SERVER
#define MINI_MUDUO_SERVER

#include "./tcp_connection.hh"

#include <string>

class Server {
  public:
    virtual auto on_connection(TcpConnection* tcp_connection) -> void = 0;
    virtual auto on_message(TcpConnection* tcp_connection, const std::string& data) -> void = 0;
};

#endif