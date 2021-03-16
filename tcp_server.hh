#ifndef MINI_MUDUO_TCP_SERVER
#define MINI_MUDUO_TCP_SERVER

#include "./def.hh"
#include "./decl.hh"
#include "./callback.hh"

#include <unordered_map>

class TcpServer : public AcceptorCallback {
  public:
    TcpServer();
    ~TcpServer() = default;

    virtual auto on_connection(int sockfd) -> void;

    auto run() -> int;

  private:
    Acceptor* mp_acceptor;
    EventLoop* mp_event_loop;
    std::unordered_map<int, TcpConnectionHandler*> m_connections;
};

#endif